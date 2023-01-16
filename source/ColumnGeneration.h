#pragma once

#include "Algorithm.h"

#include "SetPartitioning.h"
#include "SetPartitioningNew.h"

class Pricing;

class ColumnGeneration : public Algorithm
{
public:
	ColumnGeneration();
	~ColumnGeneration();

	bool run() override;

	double getObjValue() const { return solver.getObjValue(); }

	const std::vector<std::vector<Route*>>& getRoutes() const { return solver.getRoutes(); }

	const Solution& getSolution() { return solver.getSolution(); }

	const std::vector<std::vector<std::pair<int, double>>>& getRouteSolution() { return solver.getRouteSolution(); }

	void setBounds(const Bounds& bounds) { solver.setBounds(bounds); }

	void cleanModel() { solver.cleanModel(); }

	void setRoot(bool value);

	void dumpSolution(const std::vector<Route>& fixed_routes);

private:
	SetPartitioningNew solver;

	Pricing* heuristic;
	Pricing* exact;
	Pricing* backward;

	double stab_factor;
	bool stab_change;

	int depot;

	double time_left;

	Pricing* pricing = nullptr;
	Duals last_duals;

	enum State
	{
		Master,
		Heuristic,
		Exact,
		End,
		Infeasible,
	};

	bool is_root;

	State generateColumns(State state);
	void enumerateRoutes();
	void fixByReducedCost();

	void _dumpSolution(std::ostream& os, const std::vector<Route>& fixed_routes);
};
