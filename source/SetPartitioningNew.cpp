#include "SetPartitioningNew.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include "Data.h"
#include "Parameters.h"
#include "ProcessedData.h"

#include "Route.h"

#define NAMES

using namespace std;

SetPartitioningNew::SetPartitioningNew() : model(env, "EVSP")
{
	covering = !params.cg.sp;
	integer = false;

	routes.resize(data.depots.size());
	route_set.resize(data.depots.size());
	route_solution.resize(data.depots.size());
	all_routes.resize(data.depots.size());

	edge_routes.resize(data.edges.size());
	vertex_routes.resize(data.vertices.size());
	bounds = nullptr;

	if (!params.cg.no_singleton_routes) createSingletonRoutes();
	createModel();
}

SetPartitioningNew::~SetPartitioningNew()
{
	env.end();
	for (auto& vec : routes) for (Route* route : vec) delete route;
}

void SetPartitioningNew::createModel()
{
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

bool SetPartitioningNew::run()
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

void SetPartitioningNew::createLambdaVars()
{
	vars.lambdas = IloArray<IloNumVarArray>(env, data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		vars.lambdas[d] = IloNumVarArray(env, routes[d].size(), 0.0, IloInfinity);
#ifdef NAMES
		for (int r = 0; r < (int)routes[d].size(); r++)
		{
			stringstream ss;
			ss << "l(" << d << ")(" << r << ")";
			vars.lambdas[d][r].setName(ss.str().c_str());
		}
#endif
	}
}

void SetPartitioningNew::createFleetVars()
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

void SetPartitioningNew::createArtificialVars()
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

void SetPartitioningNew::createObjective()
{
	IloExpr expr(env);
	obj = IloMinimize(env, expr);
	expr.end();
	model.add(obj);
	setOriginalObjective();
}

void SetPartitioningNew::createConstraints()
{
	ranges = IloRangeArray(env, data.vertices.size());
	for (int v = 0; v < (int)data.vertices.size(); v++)
	{
		IloExpr expr(env);
		for (auto& info : vertex_routes[v])
			expr += vars.lambdas[info.depot][info.index];

		IloRange range;
		if (data.vertices[v].type == Vertex::Depot)
			range = IloRange(expr + vars.artificials[v] - vars.fleet[v] == 0);
		else if (data.vertices[v].type == Vertex::Station)
			range = IloRange(expr + vars.artificials[v] >= 0);
		else if (params.cg.sp)
			range = IloRange(expr + vars.artificials[v] == 1);
		else
			range = IloRange(expr + vars.artificials[v] >= 1);
		expr.end();
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

void SetPartitioningNew::createBoundsRanges()
{
	bounds_ranges = IloRangeArray(env);
	setBounds();
}

void SetPartitioningNew::createSingletonRoutes()
{
	for (const Vertex& vertex : data.vertices)
	{
		if (vertex.type == Vertex::Task)
		{
			double min_cost = numeric_limits<double>::infinity();
			int min_depot = -1;
			int min_station_after = -1;

			for (const Depot& depot : data.depots)
			{
				int station_after = -1;

				const Edge& depot_vertex = data.edges[data.adj_indexes[depot.id][vertex.id]];
				const Edge& vertex_depot = data.edges[data.adj_indexes[vertex.id][depot.id]];

				double cost = numeric_limits<double>::infinity();
				if (depot_vertex.cost + vertex.consumption + vertex_depot.cost - data.max_charge > EPS)
				{
					for (int s : pd.stations)
					{
						const Vertex& station = data.vertices[s];
						const Edge& vertex_station = data.edges[data.adj_indexes[vertex.id][station.id]];
						const Edge& station_depot = data.edges[data.adj_indexes[station.id][depot.id]];

						if (depot_vertex.cost + vertex.consumption + vertex_station.cost - data.max_charge > EPS)
						{
							continue;
						}
						else if (depot_vertex.cost + vertex_station.cost + station_depot.cost - cost < -EPS)
						{
							cost = depot_vertex.cost + vertex_station.cost + station_depot.cost;
							station_after = station.id;
						}
					}
				}
				else cost = depot_vertex.cost + vertex_depot.cost;

				if (cost - min_cost < -EPS)
				{
					min_cost = cost;
					min_depot = depot.id;
					min_station_after = station_after;
				}
			}

			if (min_cost == numeric_limits<double>::infinity())
			{
				out << "Route to " << vertex.id << " cannot be made without recharge before visit." << endl;
				throw runtime_error("Impossible to create singleton route!");
			}

			Route* route = new Route();
			route->id = (int)routes[min_depot].size();
			route->depot = min_depot;
			route->cost = min_cost + data.vehicle_cost;
			if (min_station_after == -1)
				route->vertices = { min_depot, vertex.id, min_depot };
			else
				route->vertices = { min_depot, vertex.id, min_station_after, min_depot };
			route->is_singleton = true;
			routes[min_depot].emplace_back(route);

			edge_routes[data.adj_indexes[min_depot][vertex.id]].emplace_back(route->depot, route->id);
			vertex_routes[vertex.id].emplace_back(route->depot, route->id);
			if (min_station_after == -1)
				edge_routes[data.adj_indexes[vertex.id][min_depot]].emplace_back(route->depot, route->id);
			else
			{
				edge_routes[data.adj_indexes[vertex.id][min_station_after]].emplace_back(route->depot, route->id);
				vertex_routes[min_station_after].emplace_back(route->depot, route->id);
				edge_routes[data.adj_indexes[min_station_after][min_depot]].emplace_back(route->depot, route->id);
			}
		}
	}
}

bool SetPartitioningNew::testArtificials()
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

void SetPartitioningNew::setArtificialObjective()
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

void SetPartitioningNew::setOriginalObjective()
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

int SetPartitioningNew::insertRoutes(const std::vector<Route*>& routes, bool stab /*= false*/)
{
	int total = 0;
	for (auto route : routes) if (insertRoute(route, stab)) total++;
	return total;
}

bool SetPartitioningNew::insertRoute(Route* route, bool allow_duplicate /*= false*/)
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
		vertex_routes[current].emplace_back(route->depot, id);
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

void SetPartitioningNew::buildSolution()
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

void SetPartitioningNew::buildRouteSolution()
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

vector<double> SetPartitioningNew::buildDuals(IloRangeArray ranges)
{
	std::vector<double> ret(ranges.getSize());
	IloNumArray temp(env, ranges.getSize());
	cplex.getDuals(temp, ranges);

	for (int i = 0; i < (int)ret.size(); i++)
		ret[i] = temp[i];
	return ret;
}

void SetPartitioningNew::buildDuals()
{
	if (duals_built) return;
	duals.vertices = buildDuals(ranges);
	duals.bounds = buildDuals(bounds_ranges);
	duals_built = true;
}

bool SetPartitioningNew::cleanModel()
{
	vector<vector<bool>> routes_erased(data.depots.size());
	for (int d = 0; d < (int)data.depots.size(); d++)
		routes_erased[d].resize(routes[d].size(), false);

	for (auto& bound : bounds->info)
	{
		if (bound.key.type != VarType::Task) continue;
		for (auto& info : vertex_routes[bound.key.id])
			routes_erased[info.depot][info.index] = true;
	}

	edge_routes.clear();
	edge_routes.resize(data.edges.size());
	vertex_routes.clear();
	vertex_routes.resize(data.vertices.size());

	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		route_set[d].clear();
		vector<Route*> routes_kept;

		for (int r = 0; r < (int)routes[d].size(); r++)
		{
			if ((routes_erased[d][r]) && (!routes[d][r]->is_singleton))
			{
				if ((!params.cg.mip) || (!all_routes[d].insert(routes[d][r]).second))
					delete routes[d][r];
				continue;
			}

			routes[d][r]->id = (int)routes_kept.size();
			routes_kept.push_back(routes[d][r]);
			route_set[d].insert(routes[d][r]);

			for (int v = 1; v < (int)routes[d][r]->vertices.size(); v++)
			{
				int from = routes[d][r]->vertices[v - 1];
				int to = routes[d][r]->vertices[v];
				edge_routes[data.adj_indexes[from][to]].emplace_back(d, routes[d][r]->id);
				vertex_routes[to].emplace_back(d, routes[d][r]->id);
			}
		}
		routes[d] = routes_kept;
	}

	cplex.end();
	model.end();
	env.end();

	env = IloEnv();
	model = IloModel(env, "EVSP");
	createModel();

	return true;
}

void SetPartitioningNew::setBounds(const Bounds& new_bounds)
{
	bounds = &new_bounds;
	bounds_ranges.endElements();
	bounds_ranges.clear();
	setBounds();
}

void SetPartitioningNew::setBounds()
{
	if (bounds == nullptr) return;

	for (int d = 0; d < (int)data.depots.size(); d++)
		if (!params.cg.fix_fleet)
			if (params.cg.relax_fleet)
				vars.fleet[d].setBounds(0.0, IloInfinity);
			else
				vars.fleet[d].setBounds(0.0, data.depots[d].vehicles);

	for (int v = 0; v < (int)data.vertices.size(); v++)
		if (data.vertices[v].type == Vertex::Task)
			ranges[v].setBounds(1.0, (covering ? IloInfinity : 1.0));

	for (const auto& bound : bounds->info)
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

void SetPartitioningNew::loadAllRoutes()
{
	for (int d = 0; d < (int)data.depots.size(); d++)
		for (auto& route : all_routes[d])
			insertRoute(route, true);
}

void SetPartitioningNew::goInt()
{
	for (int d = 0; d < (int)data.depots.size(); d++)
		model.add(IloConversion(env, vars.lambdas[d], IloNumVar::Bool));
	cplex.setOut(out);
	integer = true;
}

void SetPartitioningNew::dumpModel()
{
	static int index = 0;
	stringstream ss;
	ss << "lp/" << data.name << "-" << index++ << "-sp.lp";
	cplex.exportModel(ss.str().c_str());
}

void SetPartitioningNew::dumpDualInformation()
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

void SetPartitioningNew::goPartitioning()
{
	if (!covering) return;
	for (int v = 0; v < (int)data.vertices.size(); v++)
		if (data.vertices[v].type == Vertex::Task)
			ranges[v].setUB(1.0);
	covering = false;
	out << "Going to partitioning..." << endl;
}
