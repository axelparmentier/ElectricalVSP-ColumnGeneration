#include "ProcessedData.h"

#include <iostream>
#include <functional>
#include <algorithm>

#include "Data.h"

using namespace std;

ProcessedData::ProcessedData() : data(Data::getInstance())
{
	buildLists();
	buildTopologicalOrdering();
	buildBackwardsInformation();
}

void ProcessedData::buildLists()
{
	for (const Vertex& vertex : data.vertices)
		switch (vertex.type)
		{
		case Vertex::Station: stations.emplace_back(vertex.id); break;
		case Vertex::Task: tasks.emplace_back(vertex.id); break;
		}
}

void ProcessedData::buildTopologicalOrdering()
{
	top_order.resize(data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		vector<bool> in_stack(data.vertices.size(), false);
		vector<bool> visited(data.vertices.size(), false);
		for (int d2 = 0; d2 < (int)data.depots.size(); d2++)
			if (d != d2) visited[d2] = true;

		function<void(int)> topSort = [&](int next)->void
		{
			if (data.vertices[next].type == Vertex::Station) return;
			if (in_stack[next])
			{
				cout << "Houston, we have a cycle!" << endl;
				throw;
			}
			if (visited[next]) return;

			if (next != d) in_stack[next] = true;
			visited[next] = true;

			const Vertex& vertex = data.vertices[next];
			for (int e : vertex.edges)
			{
				const Edge& edge = data.edges[e];
				topSort(edge.to);
			}
			top_order[d].push_back(next);
			in_stack[next] = false;
		};

		topSort(d);
		reverse(top_order[d].begin(), top_order[d].end());
	}
}

void ProcessedData::buildBackwardsInformation()
{
	back_edges.resize(data.vertices.size());
	for (const auto& vertex : data.vertices)
		for (int e : vertex.edges)
			back_edges[data.edges[e].to].push_back(e);

	back_top_order.resize(data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		vector<bool> in_stack(data.vertices.size(), false);
		vector<bool> visited(data.vertices.size(), false);
		for (int d2 = 0; d2 < (int)data.depots.size(); d2++)
			if (d != d2) visited[d2] = true;

		function<void(int)> backTopSort = [&](int prev)->void
		{
			if (data.vertices[prev].type == Vertex::Station) return;
			if (in_stack[prev])
			{
				cout << "Houston, we have a cycle!" << endl;
				throw;
			}
			if (visited[prev]) return;

			if (prev != d) in_stack[prev] = true;
			visited[prev] = true;

			for (int e : back_edges[prev])
			{
				const Edge& edge = data.edges[e];
				backTopSort(edge.from);
			}
			back_top_order[d].push_back(prev);
			in_stack[prev] = false;
		};

		backTopSort(d);
		reverse(back_top_order[d].begin(), back_top_order[d].end());
	}
}
