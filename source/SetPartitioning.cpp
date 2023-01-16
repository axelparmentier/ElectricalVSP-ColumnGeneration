#include "SetPartitioning.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include "Data.h"
#include "Parameters.h"
#include "Route.h"

#define NAMES

using namespace std;

SetPartitioning::SetPartitioning() : model(env, "MDVSPTW")
{
	covering = !params.cg.sp;
	integer = false;

	routes.resize(data.depots.size());
	route_set.resize(data.depots.size());
	route_solution.resize(data.depots.size());
	all_routes.resize(data.depots.size());

	edge_routes.resize(data.edges.size());
	bounds = nullptr;

	createLambdaVars();
	createFleetVars();
	createArtificialVars();

	createObjective();

	createConstraints();
	createBoundsRanges();

	cplex = IloCplex(model);
	cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::IntParam::Threads, 1);
	cplex.setParam(IloCplex::IntParam::RootAlg, IloCplex::Primal);
	cplex.setParam(IloCplex::NumParam::EpGap, 1e-7);
}

SetPartitioning::~SetPartitioning()
{
	env.end();
	for (auto& vec : routes) for (Route* route : vec) delete route;
}

bool SetPartitioning::run()
{
	while (true)
	{
		route_solution_built = false;
		solution_built = false;
		duals_built = false;

#ifdef _DEBUG
		dumpModel();
#endif

		IloBool status = cplex.solve();
		if (status == IloTrue)
		{
			obj_val = cplex.getObjValue();
			if ((obj_val < EPS) && (has_artificials))// && (testArtificials()))
			{
				setOriginalObjective();
				continue;
			}
			return true;
		}
		else if ((!has_artificials) && (!integer))
			setArtificialObjective();
		else break;
	}
	return false;
}

void SetPartitioning::createLambdaVars()
{
	vars.lambdas = IloArray<IloNumVarArray>(env, data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
		vars.lambdas[d] = IloNumVarArray(env);
}

void SetPartitioning::createFleetVars()
{
	vars.fleet = IloNumVarArray(env, data.depots.size(), 0.0, IloInfinity);
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		if (params.cg.fix_fleet)
			vars.fleet[d].setBounds(data.depots[d].vehicles, data.depots[d].vehicles);
		else if (!params.cg.relax_fleet)
			vars.fleet[d].setBounds(0.0, data.depots[d].vehicles);
#ifdef NAMES
		stringstream ss;
		ss << "z(" << d << ")";
		vars.fleet[d].setName(ss.str().c_str());
#endif
	}
}

void SetPartitioning::createArtificialVars()
{
	vars.artificials = IloNumVarArray(env, data.vertices.size(), 0.0, IloInfinity);
#ifdef NAMES
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		stringstream ss;
		ss << "a(" << v << ")";
		vars.artificials[v].setName(ss.str().c_str());
	}
#endif
}

void SetPartitioning::createObjective()
{
	IloExpr expr(env);
	obj = IloMinimize(env, expr);
	expr.end();
	model.add(obj);
	setOriginalObjective();
}

void SetPartitioning::createConstraints()
{
	ranges = IloRangeArray(env, data.vertices.size());
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		IloRange range;
		if (data.vertices[v].type == Vertex::Depot)
			range = IloRange(vars.artificials[v] - vars.fleet[v] == 0);
		else if (data.vertices[v].type == Vertex::Station)
			range = IloRange(vars.artificials[v] >= 0);
		else if (params.cg.sp)
			range = IloRange(vars.artificials[v] == 1);
		else
			range = IloRange(vars.artificials[v] >= 1);
#ifdef NAMES
		stringstream ss;
		ss << (data.vertices[v].type == Vertex::Depot ? "Depot(" : 
			(data.vertices[v].type == Vertex::Station ? "Station(" : "Task(")) << v << ")";
		range.setName(ss.str().c_str());
#endif
		ranges[v] = range;
	}
	model.add(ranges);
}

void SetPartitioning::createBoundsRanges()
{
	bounds_ranges = IloRangeArray(env);
}

bool SetPartitioning::testArtificials()
{
	IloCplex::BasisStatusArray basis(env);
	cplex.getBasisStatuses(basis, vars.artificials);
	IloNumArray solution(env);
	cplex.getValues(solution, vars.artificials);

	for (int v = 0; v < (int)data.vertices.size(); v++)
		if ((basis[v] == IloCplex::Basic) && (solution[v] > EPS))
			return false;
	return true;
}

void SetPartitioning::setArtificialObjective()
{
	obj.setConstant(0.0);
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		obj.setLinearCoef(vars.artificials[v], 1.0);
		vars.artificials[v].setUB(IloInfinity);
	}
	for (int d = 0; d < (int)data.depots.size(); d++)
		for (int r = 0; r < (int)routes[d].size(); r++)
			obj.setLinearCoef(vars.lambdas[d][r], 0.0);
	has_artificials = true;
	out << "Introducing artificial variables..." << endl;
}

void SetPartitioning::setOriginalObjective()
{
	obj.setConstant(data.service_cost);
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		obj.setLinearCoef(vars.artificials[v], 0.0);
		vars.artificials[v].setUB(0.0);
	}
	for (int d = 0; d < (int)data.depots.size(); d++)
		for (int r = 0; r < (int)routes[d].size(); r++)
			obj.setLinearCoef(vars.lambdas[d][r], routes[d][r]->cost);
	has_artificials = false;
	out << "Removing artificial variables..." << endl;
}

int SetPartitioning::insertRoutes(const std::vector<Route*>& routes, bool stab /*= false*/)
{
	int total = 0;
	for (auto route : routes) if (insertRoute(route, stab)) total++;
	return total;
}

bool SetPartitioning::insertRoute(Route* route, bool allow_duplicate /*= false*/)
{
	int id = (int)routes[route->depot].size();
	route->id = id;

	auto it = route_set[route->depot].insert(route);
	if (!it.second)
	{
		if (!allow_duplicate) cerr << "Duplicate Route(" << (*it.first)->id << "): " << "(" << routes[route->depot][(*it.first)->id]->cost
			<< " x " << route->cost << ", " << route->red_cost << "): " << route->vertices << endl;
		//delete route; //TODO: check this.
		return false;
	}

	IloNumColumn col = ranges[route->depot](1.0);
	if (!has_artificials) col += obj(route->cost);

	for (int v = 1; v < (int)route->vertices.size(); v++)
	{
		int current = route->vertices[v];
		int last = route->vertices[v - 1];
		int edge = data.adj_indexes[last][current];
		edge_routes[edge].emplace_back(route->depot, id);
		if (current != route->depot) col += ranges[current](1.0);
		if ((bounds != nullptr) && (bounds->edges[edge] != -1)) col += bounds_ranges[bounds->edges[edge]](1.0);
	}

#ifdef NAMES
	stringstream ss;
	ss << "l(" << route->depot << ")(" << id << ")";
	vars.lambdas[route->depot].add(IloNumVar(col, 0.0, IloInfinity, IloNumVar::Float, ss.str().c_str()));
#else
	vars.lambdas[route->depot].add(IloNumVar(col, 0.0, IloInfinity, IloNumVar::Float));
#endif
	col.end();
	routes[route->depot].push_back(route);

	return true;
}

void SetPartitioning::buildSolution()
{
	if (solution_built) return;
	if (!route_solution_built) buildRouteSolution();

	solution.clear();
	for (int d = 0; d < (int)data.depots.size(); d++)
		for (int r = 0; r < (int)route_solution[d].size(); r++)
		{
			int route = route_solution[d][r].first;
			double value = route_solution[d][r].second;
			solution[{ d, route, VarType::Vehicle }] += value;

			for (int v = 1; v < (int)routes[d][route]->vertices.size(); v++)
			{
				int from = routes[d][route]->vertices[v - 1];
				int to = routes[d][route]->vertices[v];
				int edge = data.adj_indexes[from][to];
				solution[{ -1, edge, VarType::FullEdge }] += value;
			}
		}
				
	IloNumArray sol(env);
	cplex.getValues(sol, vars.fleet);
	for (int d = 0; d < (int)data.depots.size(); d++)
		solution[{ d, -1, VarType::Fleet }] += sol[d];

	solution_built = true;
}

void SetPartitioning::buildRouteSolution()
{
	if (route_solution_built) return;

	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		route_solution[d].clear();

		IloNumArray sol(env);
		cplex.getValues(sol, vars.lambdas[d]);
		for (int r = 0; r < (int)routes[d].size(); r++)
			if (sol[r] > EPS) route_solution[d].emplace_back(r, sol[r]);
	}
	route_solution_built = true;
}

vector<double> SetPartitioning::buildDuals(IloRangeArray ranges)
{
	std::vector<double> ret(ranges.getSize());
	IloNumArray temp(env, ranges.getSize());
	cplex.getDuals(temp, ranges);

	for (int i = 0; i < (int)ret.size(); i++)
		ret[i] = temp[i];
	return ret;
}

void SetPartitioning::buildDuals()
{
	if (duals_built) return;
	duals.vertices = buildDuals(ranges);
	duals.bounds = buildDuals(bounds_ranges);
	duals_built = true;
}

bool SetPartitioning::cleanModel()
{
	bool done = false;
	int limit = 1;

	IloArray<IloNumArray> red_costs(env, data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		red_costs[d] = IloNumArray(env);
		cplex.getReducedCosts(red_costs[d], vars.lambdas[d]);
	}

	edge_routes.clear();
	edge_routes.resize(data.edges.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		if (routes[d].empty()) continue;

		route_set[d].clear();
		vector<Route*> routes_kept;
		IloNumVarArray lambdas_kept(env);

		vector<pair<double, int>> order(routes[d].size());
		for (int r = 0; r < (int)routes[d].size(); r++)
			order[r] = { red_costs[d][r], r };
		sort(order.begin(), order.end());

		for (int r = 0; r < (int)order.size(); r++)
			if ((r < limit) || (order[r].first < 1.0 + EPS))
			{
				int id = order[r].second;
				lambdas_kept.add(vars.lambdas[d][id]);
				routes_kept.push_back(routes[d][id]);
				routes[d][id]->id = r;
				route_set[d].insert(routes[d][id]);

				if (order[r].first < 1.0 + EPS) limit = (int)(r * (1.0 + params.cg.clean_factor));
#ifdef NAMES
				stringstream ss;
				ss << "l(" << routes[d][id]->depot << ")(" << routes[d][id]->id << ")";
				lambdas_kept[lambdas_kept.getSize() - 1].setName(ss.str().c_str());
#endif
				for (int v = 1; v < (int)routes[d][id]->vertices.size(); v++)
				{
					int from = routes[d][id]->vertices[v - 1];
					int to = routes[d][id]->vertices[v];
					edge_routes[data.adj_indexes[from][to]].emplace_back(d, r);
				}
			}
			else
			{
				done = true;
				int id = order[r].second;
				vars.lambdas[d][id].end();
				if ((!params.cg.mip) || (!all_routes[d].insert(routes[d][id]).second))
					delete routes[d][id];
			}

		routes[d] = routes_kept;
		vars.lambdas[d] = lambdas_kept;
	}
	return done;
}

void SetPartitioning::setBounds(const Bounds& new_bounds)
{
	bounds = &new_bounds;
	bounds_ranges.endElements();
	bounds_ranges.clear();

	for (int d = 0; d < (int)data.depots.size(); d++)
		if (!params.cg.fix_fleet)
			if (params.cg.relax_fleet)
				vars.fleet[d].setBounds(0.0, IloInfinity);
			else
				vars.fleet[d].setBounds(0.0, data.depots[d].vehicles);

	for (int v = 0; v < (int)data.vertices.size(); v++)
		if (data.vertices[v].type == Vertex::Task)
			ranges[v].setBounds(1.0, (covering ? IloInfinity : 1.0));

	for (const auto& bound : new_bounds.info)
	{
		IloExpr expr(env);
		if (bound.key.type == Fleet)
		{
			bounds_ranges.add(expr <= 0);
			vars.fleet[bound.key.depot].setBounds(bound.value.lb, bound.value.ub == numeric_limits<int>::max() ? IloInfinity : bound.value.ub);
		}
		else if (bound.key.type == FullEdge)
		{
			for (const RouteId& route : edge_routes[bound.key.id])
				expr += vars.lambdas[route.depot][route.index];
			IloRange range = IloRange(bound.value.lb <= expr <= (bound.value.ub == numeric_limits<int>::max() ? IloInfinity : bound.value.ub));
#ifdef NAMES
			stringstream ss;
			const Edge& edge = data.edges[bound.key.id];
			ss << "Bound(" << edge.from << ")(" << edge.to << ")";
			range.setName(ss.str().c_str());
#endif
			bounds_ranges.add(range);
		}
		else if (bound.key.type == Task)
			ranges[bound.key.id].setBounds(0.0, 0.0);
		expr.end();
	}
	model.add(bounds_ranges);
}

void SetPartitioning::loadAllRoutes()
{
	for (int d = 0; d < (int)data.depots.size(); d++)
		for (auto& route : all_routes[d])
			insertRoute(route, true);
}

void SetPartitioning::goInt()
{
	for (int d = 0; d < (int)data.depots.size(); d++)
		model.add(IloConversion(env, vars.lambdas[d], IloNumVar::Bool));
	cplex.setOut(out);
	integer = true;
}

void SetPartitioning::dumpModel()
{
	static int index = 0;
	stringstream ss;
	ss << "lp/" << data.name << "-" << index++ << "-sp.lp";
	cplex.exportModel(ss.str().c_str());
}

void SetPartitioning::dumpDualInformation()
{
	static int index = 0;
	stringstream ss;
	ss << "out/" << data.name << "-" << index++ << "-dual.txt";
	ofstream file(ss.str(), ios::out);
	for (int v = 0; v < (int)data.vertices.size(); v++)
		file << duals.vertices[v] << endl;
	if (bounds != nullptr)
	{
		for (auto& bound : bounds->info)
			if (bound.key.type == Fleet)
				file << bound.key.id << "," << bound.value.lb << "," << bound.value.ub << endl;
			else
			{
				const Edge& edge = data.edges[bound.key.id];
				file << edge.from << "," << edge.to << "," << bound.value.lb << "," << bound.value.ub << endl;
			}
	}
	file.close();
}

void SetPartitioning::goPartitioning()
{
	if (!covering) return;
	for (int v = 0; v < (int)data.vertices.size(); v++)
		if (data.vertices[v].type == Vertex::Task)
			ranges[v].setUB(1.0);
	covering = false;
	out << "Going to partitioning..." << endl;
}
