#include "RouteEnumeration.h"

#include <algorithm>

#include "Data.h"
#include "ProcessedData.h"

#include "Route.h"
#include "BranchAndPriceStructs.h"
#include "Parameters.h"

using namespace std;

RouteEnumeration::RouteEnumeration() : gap(numeric_limits<double>::infinity()) {}

void RouteEnumeration::init()
{
	if (params.isThereVehicleWithInitialCharge){
		out << "Route enumeration not implemented to work with vehicles with initial charge" << endl;
		throw;
	}

	value = 0.0;
	labels.clear();
	closeds.clear();
	routes.clear();

	labels.resize(data.vertices.size());
	labels[depot].emplace_back(depot, 0.0, nullptr);
}

bool RouteEnumeration::do_price()
{
	init();

	for (int v : pd.top_order[depot])
	{
		const Vertex& vertex = data.vertices[v];
		for (int i = 0; i < (int)labels[v].size(); i++)
		{
			for (int e = 0; e < (int)vertex.edges.size(); e++)
			{
				const Edge& edge = data.edges[vertex.edges[e]];
				if ((edge.to != depot) && (edge.to < (int)data.depots.size())) continue;

				double red_cost = labels[vertex.id][i].red_cost + (artificials ? 0.0 : edge.cost) - duals->vertices[edge.to];
				if (red_cost + duals->vertices[edge.to] + completion_bounds[edge.to] - gap > EPS) continue;

				Label label(edge.to, red_cost, &labels[vertex.id][i]);
				if ((edge.to == depot) && (red_cost - gap < EPS))
					closeds.push_back(label);
				else
					labels[edge.to].push_back(label);
			}
		}
	}
	buildRoutes();

	return (!routes.empty());
}

void RouteEnumeration::buildRoutes()
{
	for (int r = 0; r < (int)closeds.size(); r++)
	{
		if (r == 0) value = closeds[r].red_cost;

		Route* route = new Route;
		route->depot = depot;
		route->cost = 0;
		route->vertices.push_back(depot);

		Label* next = &closeds[r];
		Label* current = closeds[r].last;
		while (true)
		{
			route->cost += data.edges[data.adj_indexes[current->task][next->task]].cost;
			route->vertices.push_back(current->task);

			if (current->task == depot) break;

			next = current;
			current = current->last;
		}
		reverse(route->vertices.begin(), route->vertices.end());
		routes.push_back(route);
	}
}
