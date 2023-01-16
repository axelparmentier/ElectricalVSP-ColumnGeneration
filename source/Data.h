#ifndef DATA_H
#define DATA_H

#include "Singleton.h"

#include <vector>

#include "Depot.h"
#include "Vertex.h"
#include "Edge.h"

#define EPS 1e-5

class Data : public Singleton<Data>
{
	friend class Singleton<Data>;
public:
	std::vector<Depot> depots;
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;

	std::vector<std::vector<int>> adj_indexes;

	int num_depots;
	int num_stations;
	int num_tasks;

	std::string name;
	std::string folder;
	double ub;

	double service_cost;
	double vehicle_cost;
	double vehicleWithInitialPosition_cost;

	int max_charge;
	double recharge_factor;

private:
	Data() = default;
};

#endif
