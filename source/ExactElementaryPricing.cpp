#include "ExactElementaryPricing.h"

#include <algorithm>

#include "Data.h"
#include "ProcessedData.h"

#include "Route.h"
#include "BranchAndPriceStructs.h"

using namespace std;

ExactElementaryPricing::ExactElementaryPricing()
{
	labels.resize(data.vertices.size());
}

void ExactElementaryPricing::init()
{
	for (int v = 0; v < (int)data.vertices.size(); v++)
		labels[v] = { (v == depot ? (artificials ? 0.0 : data.vehicle_cost) : numeric_limits<double>::infinity()), -1 };

	value = 0.0;
	closeds.clear();
	routes.clear();
}

bool ExactElementaryPricing::do_price()
{
	init();

	for (int v : pd.top_order[depot])
	{
		const Vertex& vertex = data.vertices[v];
		for (int e = 0; e < (int)vertex.edges.size(); e++)
		{
			const Edge& edge = data.edges[vertex.edges[e]];
			if ((edge.to != depot) && (edge.to < (int)data.depots.size())) continue;
			
			double red_cost = labels[vertex.id].red_cost + (artificials ? 0.0 : edge.cost) - duals->vertices[edge.to];
			if (red_cost - labels[edge.to].red_cost < -EPS)
				labels[edge.to] = { red_cost, vertex.id };

			if ((edge.to == depot) && (red_cost < -EPS))
				closeds.push_back({ red_cost, vertex.id });
		}
	}

	sort(closeds.begin(), closeds.end(), [](const Label& left, const Label& right)
	{
		return (left.red_cost < right.red_cost);
	});

	for (int r = 0; r < min((int)closeds.size(), 5); r++)
	{
		if (r == 0) value = closeds[r].red_cost;

		Route* route = new Route;
		route->depot = depot;
		route->cost = data.vehicle_cost;
		route->vertices.push_back(depot);

		int next = depot;
		int current = closeds[r].last;
		while (true)
		{
			route->cost += data.edges[data.adj_indexes[current][next]].cost;
			route->vertices.push_back(current);

			if (current == depot) break;

			next = current;
			current = labels[current].last;
		}
		reverse(route->vertices.begin(), route->vertices.end());
		routes.push_back(route);
	}
	buildCompletionBounds();

	return (!routes.empty());
}

void ExactElementaryPricing::buildCompletionBounds()
{
	completion_bounds.resize(data.vertices.size(), numeric_limits<double>::infinity());
	for (int v = 0; v < (int)data.vertices.size(); v++)
		completion_bounds[v] = labels[v].red_cost;
}
