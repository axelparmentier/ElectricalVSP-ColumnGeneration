#include "VSPLoader.h"

#include <fstream>
#include <iostream>

#include "Data.h"
#include "Parameters.h"

using namespace std;

bool VSPLoader::load()
{
	data.name = params.file_name.substr(params.file_name.find_last_of("/") + 1);
	data.name = data.name.substr(0, data.name.find_last_of("."));

	if (!params.silent) cout << "Loading instance " << data.name << "... ";

	ifstream in(params.file_name, ios::in);

	if (!in.good())
	{
		cerr << "Could not find instance " << params.file_name << endl;
		return false;
	}

	int depots;
	int customers;
	in >> depots >> customers;

	data.depots.resize(depots);
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		data.depots[d].id = d;
		in >> data.depots[d].vehicles;
	}

	data.vertices.resize(depots + customers);
	data.adj_indexes.resize(data.vertices.size());
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		data.vertices[v].id = v;
		data.adj_indexes[v].resize(data.vertices.size(), -1);
		for (int w = 0; w < (int)data.vertices.size(); w++)
		{
			int cost;
			in >> cost;

			if (cost != -1)
			{
				int id = (int)data.edges.size();
				Edge edge(id, v, w, cost);

				data.edges.push_back(edge);
				data.vertices[v].edges.push_back(id);
				data.adj_indexes[v][w] = id;
			}
		}
	}
	data.ub = numeric_limits<double>::infinity();

	if (!params.silent) cout << "done!" << endl;
	in.close();

	loadUBs();

	return true;
}
