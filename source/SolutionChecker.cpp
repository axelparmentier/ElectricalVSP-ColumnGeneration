#include "SolutionChecker.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Data.h"
#include "Route.h"
#include "Templates.h"

using namespace std;

SolutionChecker::SolutionChecker() : os(out) {}
SolutionChecker::SolutionChecker(ostream& os) : os(os) {}

bool SolutionChecker::run()
{
	loadFromFile();
	return check();
}

bool SolutionChecker::run(const vector<const Route*>& routes, double cost)
{
	this->cost = cost;
	this->routes = routes;
	return check();
}

bool SolutionChecker::run(const Route* route)
{
	this->cost = route->cost;
	this->routes.clear();
	this->routes.push_back(route);
	return check();
}

bool SolutionChecker::check()
{
	vector<bool> marked(data.vertices.size(), false);
	double total_cost = data.service_cost;
	for (auto& route : routes)
	{
		os << route->vertices << endl;
		os << "Checking route " << route->id << " - depot = " << route->depot << ":" << endl;
		double route_cost = data.vehicle_cost;
		double time = 0.0;
		double charge = 0.0;
		for (int v = 1; v < (int)route->vertices.size(); v++)
		{
			if (route->vertices[v - 1] == route->vertices[v]) continue;
			
			const Vertex& vertex = data.vertices[route->vertices[v]];
			const Vertex& last = data.vertices[route->vertices[v - 1]];
			const Edge& edge = data.edges[data.adj_indexes[route->vertices[v - 1]][route->vertices[v]]];
			
			if ((vertex.type != Vertex::Station) && (last.type == Vertex::Station))
			{
				int w = v - 2;
				while (w >= 0)
				{
					if ((data.vertices[route->vertices[w]].type == Vertex::Station) && (route->vertices[w] != last.id))
						os << "Different stations in a row." << endl;
					if (data.vertices[route->vertices[w]].type != Vertex::Station) break;
					w--;
				}
				const Vertex& last_task = data.vertices[route->vertices[w]];
				const Edge& last_edge = data.edges[data.adj_indexes[last_task.id][last.id]];
				double recharge = (vertex.id == route->depot ? vertex.due : vertex.ready) - (last_task.id == route->depot ? last_task.ready : last_task.due) - last_edge.cost - edge.cost;
				if (recharge < -EPS)
				{
					os << "Negative recharge!" << endl;
					cout << "Negative recharge!" << endl;
				}
				charge = max(0.0, charge - recharge / data.recharge_factor);
			}

			route_cost += edge.cost;
			charge += edge.cost;
			if (vertex.type == Vertex::Task)
			{
				if (time + edge.cost - vertex.ready > EPS)
				{
					os << "Impossible to reach task " << vertex.id << " on time!" << endl;
					cout << "Impossible to reach task " << vertex.id << " on time!" << endl;
				}
				time = vertex.due;
				charge += vertex.consumption;
			}
			else
				time += edge.cost;

			if (charge - data.max_charge > EPS)
			{
				os << "Charge exceeded on task " << vertex.id << "!" << endl;
				cout << "Charge exceeded on task " << vertex.id << "!" << endl;
			}

			marked[vertex.id] = true;
		}
		os << "Cost = " << route_cost << endl;
		total_cost += route_cost;
	}

	os << "Total instance cost = " << total_cost << " x Original = " << cost << endl;

	bool ok = true;
	for (int v = 0; v < (int)data.vertices.size(); v++)
		if ((data.vertices[v].type == Vertex::Task) && (!marked[v]))
		{
			os << "Task " << v << " was not visited!" << endl;
			cout << "Task " << v << " was not visited!" << endl;
			ok = false;
		}
	if (ok) os << "All tasks visited!" << endl;

	return true;
}

bool SolutionChecker::loadFromFile()
{
	char folder_sep = '/';
#ifdef _WIN32
	folder_sep = '\\';
#endif
	string file_name = data.folder + folder_sep + data.name + "-sol.txt";

	ifstream in(file_name);
	if (!in.good())
	{
		cerr << "Could not find instance " << file_name << endl;
		return false;
	}

	int n;
	in >> n;
	in >> cost;

	for (int r = 0; r < n; r++)
	{
		Route* route = new Route();
		in >> route->id;
		route->depot = -1;
		while (true)
		{
			int vertex;
			in >> vertex;
			route->vertices.push_back(vertex);
			if (vertex == route->depot) break;
			if ((int)route->vertices.size() == 1) route->depot = vertex;
		}
		routes.push_back(route);
	}

	in.close();
	return true;
}
