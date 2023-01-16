#include "BackwardPricing.h"

#include <algorithm>

#include "Data.h"
#include "ProcessedData.h"

#include "Route.h"
#include "BranchAndPriceStructs.h"

using namespace std;

BackwardPricing::BackwardPricing()
{
	labels.resize(data.vertices.size());
}

void BackwardPricing::init()
{
	for (int v = 0; v < (int)data.vertices.size(); v++)
		labels[v] = { (v == depot ? 0.0 : numeric_limits<double>::infinity()), -1 };

	value = 0.0;
	closeds.clear();
	routes.clear();
}

bool BackwardPricing::do_price()
{
	init();

	for (int v : pd.back_top_order[depot])
	{
		for (int e = 0; e < (int)pd.back_edges[v].size(); e++)
		{
			const Edge& edge = data.edges[pd.back_edges[v][e]];
			if ((edge.from != depot) && (edge.from < (int)data.depots.size())) continue;

			double red_cost = labels[v].red_cost + (artificials ? 0.0 : edge.cost) - (duals->vertices)[edge.from];
			if (red_cost - labels[edge.from].red_cost < -EPS)
				labels[edge.from] = { red_cost, v };

			if ((edge.from == depot) && (red_cost < -EPS))
				closeds.push_back({ red_cost, v });
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
		route->cost = 0;
		route->vertices.push_back(depot);

		int prev = depot;
		int current = closeds[r].next;
		while (true)
		{
			route->cost += data.edges[data.adj_indexes[prev][current]].cost;
			route->vertices.push_back(current);

			if (current == depot) break;

			prev = current;
			current = labels[current].next;
		}
		routes.push_back(route);
	}
	buildCompletionBounds();

	return (!routes.empty());
}

void BackwardPricing::buildCompletionBounds()
{
	completion_bounds.resize(data.vertices.size(), numeric_limits<double>::infinity());
	for (int v = 0; v < (int)data.vertices.size(); v++)
		completion_bounds[v] = labels[v].red_cost;
}
