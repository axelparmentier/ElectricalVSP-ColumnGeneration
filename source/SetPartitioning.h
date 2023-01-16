#pragma once

#include "InputUser.h"

#include <set>
#include <vector>

#include <ilcplex/ilocplex.h>

#include "BranchAndPriceStructs.h"

#include "Templates.h"

class Route;

class SetPartitioning : public InputUser
{
public:
	SetPartitioning();
	~SetPartitioning();

	bool run();

	double getObjValue() const { return obj_val; }

	bool hasArtificials() const { return has_artificials; }

	int insertRoutes(const std::vector<Route*>& routes, bool stab = false);
	bool insertRoute(Route* route, bool allow_duplicate = false);

	const std::vector<std::vector<Route*>>& getRoutes() const { return routes; }
	const std::vector<std::vector<std::pair<int, double>>>& getRouteSolution() { if (!route_solution_built) buildRouteSolution(); return route_solution; }
	const Solution& getSolution() { if (!solution_built) buildSolution(); return solution; }
	const Duals& getDuals() { if (!duals_built) buildDuals(); return duals; }

	void setBounds(const Bounds& new_bounds);
	const Bounds& getBounds() const { return *bounds; }

	void loadAllRoutes();
	void goInt();

	bool cleanModel();
	void dumpModel();

	void dumpDualInformation();

	bool isCovering() const { return covering; }
	void goPartitioning();

private:
	IloEnv env;
	IloModel model;
	IloCplex cplex;

	struct Variables
	{
		IloArray<IloNumVarArray> lambdas;
		IloNumVarArray fleet;
		IloNumVarArray artificials;
	} vars;

	IloObjective obj;

	IloRangeArray ranges;
	IloRangeArray bounds_ranges;

	std::vector<std::vector<std::pair<int, double>>> route_solution;
	Solution solution;

	Duals duals;

	std::vector<std::vector<Route*>> routes;
	std::vector<std::set<Route*, Less<Route*>>> route_set;
	std::vector<std::set<Route*, Less<Route*>>> all_routes;

	class RouteId
	{
	public:
		RouteId(int depot, int index) : depot(depot), index(index) {}
		int depot;
		int index;
	};
	std::vector<std::vector<RouteId>> edge_routes;

	const Bounds* bounds;

	double obj_val;
	bool has_artificials;
	bool route_solution_built;
	bool solution_built;
	bool duals_built;

	bool covering;
	bool integer;

	void createLambdaVars();
	void createFleetVars();
	void createArtificialVars();

	void createObjective();

	void createConstraints();
	void createBoundsRanges();

	bool testArtificials();
	void setArtificialObjective();
	void setOriginalObjective();

	void buildRouteSolution();
	void buildSolution();
	std::vector<double> buildDuals(IloRangeArray ranges);
	void buildDuals();
};
