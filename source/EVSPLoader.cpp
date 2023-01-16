#include "EVSPLoader.h"

#include <fstream>
#include <iostream>

#include "Data.h"
#include "Parameters.h"

using namespace std;

bool EVSPLoader::load()
{
	char folder_sep = '/';
	if (params.file_name.find_last_of(folder_sep) == params.file_name.npos)
		folder_sep = '\\';

	data.folder = params.file_name.substr(0, params.file_name.find_last_of(folder_sep));
	data.name = params.file_name.substr(params.file_name.find_last_of(folder_sep) + 1);
	data.name = data.name.substr(0, data.name.find_last_of("."));

	if (!params.silent) cout << "Loading instance " << data.name << "... ";

	ifstream in(params.file_name, ios::in);

	if (!in.good())
	{
		cerr << "Could not find instance " << params.file_name << endl;
		return false;
	}

	int vehiclesWithInitialPosition = 0;
	int vehiclesPerDepot;
	int copies; //?
	data.vehicle_cost = 10000;
	data.vehicleWithInitialPosition_cost = 1000;

	if (params.isThereVehicleWithInitialCharge)
		in >> data.num_depots >> data.num_stations >> data.num_tasks >> vehiclesWithInitialPosition >> vehiclesPerDepot >> copies;
	else in >> data.num_depots >> data.num_stations >> data.num_tasks >> vehiclesPerDepot >> copies;

	if (params.isThereVehicleWithInitialCharge) data.depots.resize(data.num_depots + 1);
	else data.depots.resize(data.num_depots);
	
	for (int d = 0; d < (int)data.num_depots; d++)
	{
		data.depots[d].id = d;
		data.depots[d].vehicles = vehiclesPerDepot;
		data.depots[d].isArtificalDepotForVehiclesWithInitialCharge = false;
	}

	int artificialDepoId = -1;
	if (params.isThereVehicleWithInitialCharge)
	{
		data.depots[data.num_depots].id = data.num_depots;
		data.depots[data.num_depots].vehicles = vehiclesWithInitialPosition;
		data.depots[data.num_depots].isArtificalDepotForVehiclesWithInitialCharge = true;
		artificialDepoId = data.num_depots;
		data.num_depots += 1;
	}

	data.service_cost = 0.0;
	data.vertices.resize(data.num_depots + data.num_stations + vehiclesWithInitialPosition + data.num_tasks);
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		Vertex& vertex = data.vertices[v];
		vertex.id = v;

		if (v < data.num_depots) vertex.type = Vertex::Depot;
		else if (v < data.num_depots + data.num_stations) vertex.type = Vertex::Station;
		else if (v < data.num_depots + data.num_stations + vehiclesWithInitialPosition) vertex.type = Vertex::Vehicle;
		else vertex.type = Vertex::Task;
		vertex.initialCharge = 0;

		if (v == artificialDepoId)
		{
			vertex.ready = 0;
			vertex.due = 1440;
			vertex.duration = 0;
			vertex.consumption = 0;
			continue;
		}

		in >> vertex.ready >> vertex.due >> vertex.duration >> vertex.consumption;
		if (vertex.type == Vertex::Vehicle) in >> vertex.initialCharge;
		//data.service_cost += vertex.duration;
	}

	data.adj_indexes.resize(data.vertices.size());
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		Vertex& from = data.vertices[v];
		data.adj_indexes[v].resize(data.vertices.size(), -1); // -1 to identify when edge not created

		for (int w = 0; w < (int)data.vertices.size(); w++)
		{
			Vertex& to = data.vertices[w];

			double cost;
			if (v == artificialDepoId)
			{
				if (to.type == Vertex::Vehicle) cost = 0;
				else cost = numeric_limits<double>::infinity();
			}
			else if (w == artificialDepoId)
			{
				if (from.type == Vertex::Depot) cost = numeric_limits<double>::infinity();
				else cost = 0;
			}
			else in >> cost;

			if (v != w)
			{
				if ((from.type == Vertex::Task) && (to.type == Vertex::Task) && (from.due + cost > to.ready)) continue;
				if ((to.type == Vertex::Vehicle) && (v != artificialDepoId)) continue;
				if ((from.type == Vertex::Vehicle) && (to.type == Vertex::Depot) && (w != artificialDepoId)) continue;
				if ((from.type == Vertex::Vehicle) && (to.type == Vertex::Task) && (from.ready + cost > to.ready)) continue;

				int id = (int)data.edges.size();
				Edge edge(id, v, w, cost);

				data.edges.push_back(edge);
				data.vertices[v].edges.push_back(id);
				data.adj_indexes[v][w] = id;
			}
		}
	}

	in >> data.max_charge >> data.recharge_factor;

	if (!params.silent) cout << "done!" << endl;
	in.close();

	data.ub = numeric_limits<double>::infinity();
	if ((params.maxNumberOfDepots != numeric_limits<int>::max()) || (params.maxNumberOfStations != numeric_limits<int>::max()) || (params.maxNumberOfTasks != numeric_limits<int>::max()))
		cropInstance(vehiclesWithInitialPosition);
	else
		loadUBs();

	return true;
}

void EVSPLoader::cropInstance(int vehiclesWithInitialPosition)
{
	if ((data.num_depots > params.maxNumberOfDepots) || (data.num_stations > params.maxNumberOfStations) || (data.num_tasks > params.maxNumberOfTasks))
	{
		data.num_depots = (data.num_depots > params.maxNumberOfDepots ? params.maxNumberOfDepots : data.num_depots);
		data.num_stations = (data.num_stations > params.maxNumberOfStations ? params.maxNumberOfStations : data.num_stations);
		data.num_tasks = (data.num_tasks > params.maxNumberOfTasks ? params.maxNumberOfTasks : data.num_tasks);

		cout << "Cropping instance to "
			<< params.maxNumberOfDepots << " depots, "
			<< params.maxNumberOfStations << " stations, "
			<< params.maxNumberOfTasks << " tasks..."
			<< endl;

		int total = data.num_depots + data.num_stations + vehiclesWithInitialPosition + data.num_tasks;

		vector<int> vertex_remap(data.vertices.size(), -1);

		int total_id = 0;
		int total_depots = 0;
		int total_stations = 0;
		auto it = data.vertices.begin();
		while (it != data.vertices.end())
		{
			if ((it->type == Vertex::Depot) && (++total_depots > data.num_depots))
				it = data.vertices.erase(it);
			else if ((it->type == Vertex::Station) && (++total_stations > data.num_stations))
				it = data.vertices.erase(it);
			else
			{
				vertex_remap[it->id] = total_id;
				it->id = total_id++;
				++it;
				if (total_id >= total) break;
			}
		}
		data.depots.resize(data.num_depots);
		data.vertices.resize(total);

		data.adj_indexes.resize(total);
		for (int i = 0; i < data.adj_indexes.size(); i++)
		{
			data.adj_indexes[i].assign(total, -1);
			data.vertices[i].edges.clear();
		}

		int num_edges = 0;
		auto ite = data.edges.begin();
		while (ite != data.edges.end())
		{
			if (vertex_remap[ite->from] == -1 || vertex_remap[ite->to] == -1)
				ite = data.edges.erase(ite);
			else
			{
				ite->from = vertex_remap[ite->from];
				ite->to = vertex_remap[ite->to];

				ite->id = num_edges;
				data.vertices[ite->from].edges.push_back(num_edges);
				data.adj_indexes[ite->from][ite->to] = num_edges++;
				++ite;
			}
		}
	}
}
