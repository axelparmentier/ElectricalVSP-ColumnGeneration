#pragma once

#include <vector>

class Route
{
public:
	int id = -1;
	int depot = -1;
	double cost;
	double red_cost;
	std::vector<int> vertices;

	bool is_singleton = false;

	bool operator<(const Route& right) const
	{
		return (depot == right.depot ? (cost == right.cost ? vertices < right.vertices
			: cost < right.cost) : depot < right.depot);
	}
};
