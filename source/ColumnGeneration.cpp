#include "ColumnGeneration.h"

#include "Data.h"
#include "ProcessedData.h"
#include "Parameters.h"

#include "ExactElementaryPricing.h"
#include "BackwardPricing.h"
#include "RouteEnumeration.h"

#include "MonoidPricing.h"

#include "Route.h"
#include "SolutionChecker.h"

#include "Timer.h"
#include "Templates.h"

#define PRINT 1

using namespace std;

ColumnGeneration::ColumnGeneration() : is_root(true)
{
	heuristic = nullptr;
	switch (params.cg.pricing)
	{
	case Parameters::Exact: exact = new ExactElementaryPricing(); break;
	case Parameters::Monoid: exact = new MonoidPricing(); break;
	default: exit(-1);
	}
	backward = new BackwardPricing();

	stab_factor = 0.9;
	stab_change = false;
	depot = -1;
	time_left = params.time_limit;
}

ColumnGeneration::~ColumnGeneration()
{
	delete heuristic;
	delete exact;
}

bool ColumnGeneration::run()
{
	Timer timer;
	timer.start();

	Timer master_timer;
	Timer pricing_timer;

	out << endl << ">>> COLUMN GENERATION <<<" << endl;
	out << "/===============================================================================\\" << endl;
	out << "|  Iteration  |    Obj Value    |    Pricing    |   Gen   |   Total   |  Depot  |" << endl;
	out << "|=============|=================|===============|=========|===========|=========|" << endl;

	pricing = nullptr;

	State state = Master;
	State last_state = Master;
	static int index = 0;
	while ((state != End) && (state != Infeasible))
	{
		if (timer.lap() > time_left) break;

		last_state = state;
		switch (state)
		{
		case Master:
		{
			master_timer.start();
			bool had_artificials = solver.hasArtificials();
			if (solver.run())
			{
				//if ((!params.cg.no_clean) && (index > 0) && (index % 500 == 0))
				//	if (solver.cleanModel())
				//		solver.run();

				//if ((!solver.hasArtificials()) && (index % 100 == 0))
				//	fixByReducedCost();

				if (!had_artificials && solver.hasArtificials())
				{
					stab_factor = 0.9;
					stab_change = true;
				}

				state = Exact;
				if (index++ % PRINT == 0)
				{
					out << "|" << setw(11) << index << "  |  ";
					out << setprecision(1) << fixed << setw(13) << solver.getObjValue() << "  |  ";
				}
			}
			else state = Infeasible;
			master_timer.stop();
			break;
		}
		case Heuristic: case Exact:
		{
			pricing_timer.start();
			state = generateColumns(state);
			pricing_timer.stop();
			break;
		}
		}

		if ((state == Master) || (state == End))
		{
			if (((last_state == Heuristic) || (last_state == Exact)) && ((index - 1) % PRINT == 0))
			{
				out << setw(11) << pricing->getValue() << "  |  ";
				out << setw(5) << pricing->getRoutes().size() << "  |  ";
				out << setw(7) << solver.getRoutes()[depot].size() << "  |  ";
				out << setw(5) << depot << "  |  ";
				out << endl;
			}
			if (stab_change)
			{
				out << "[CG] Stabilization factor was set to (" << stab_factor << ")." << endl;
				stab_change = false;
			}
		}

		//if (state == End)
		//	enumerateRoutes();
	}
	out << "\\===============================================================================/" << endl;

	last_duals = Duals();

	//printRouteSolution();
	out << data.name << ",cg," << solver.getObjValue() << "," << timer.lap() << "," << master_timer.get() << "," << pricing_timer.get() << "," << endl;

	if ((params.cg.mip) && (is_root))
	{
		solver.loadAllRoutes();
		solver.goInt();
		if (solver.run())
			out << data.name << ",mip," << solver.getObjValue() << "," << timer.lap() << endl;
		else
			out << data.name << ",mip," << numeric_limits<double>::infinity() << "," << timer.lap() << endl;
	}

	//solver.dumpModel();
	//solver.dumpDualInformation();

	return ((state == End) && (!solver.hasArtificials()));
}

ColumnGeneration::State ColumnGeneration::generateColumns(State state)
{
	depot = (depot + 1) % data.depots.size();

	auto& duals = solver.getDuals();
	if (stab_factor > EPS)
	{
		if (last_duals.vertices.empty()) last_duals = duals;
		last_duals.vertices = last_duals.vertices * stab_factor + duals.vertices * (1 - stab_factor);
		last_duals.bounds = last_duals.bounds * stab_factor + duals.bounds * (1 - stab_factor);
	}
	else last_duals = duals;

	// DEBUG!
	//for (int v = 0; v < (int)data.vertices.size(); v++)
	//	if (data.vertices[v].type == Vertex::Station)
	//		if (last_duals.vertices[v] > EPS)
	//		{
	//			out << "[*ERROR*]: Station " << v << " has a non-zero dual value!" << endl;
	//			exit(-1);
	//		}
	// DEBUG!

	bool found = false;
	int last_depot = depot;
	while (true)
	{
		pricing = exact;
		
		int total_inserted = 0;
		//TODO: Change to pass all duals!
		const auto& bounds = solver.getBounds();
		if (pricing->price(depot, last_duals, bounds, solver.hasArtificials()))
		{
			auto& routes = pricing->getRoutes();
			total_inserted = solver.insertRoutes(routes, stab_factor > EPS);
			//DEBUG! It is not right anymore, because now duals have bounds.
			if ((stab_factor <= EPS) && (total_inserted != (int)routes.size()))
			{
				cerr << "Duplicated route found..." << endl;
				for (int r = 0; r < (int)routes.size(); r++)
				{
					int last = depot;
					double red_cost = (pricing->getArtificials() ? 0 : data.vehicle_cost);
					double cost = data.vehicle_cost;
					cerr << routes[r]->depot;
					for (int v = 1; v < (int)routes[r]->vertices.size(); v++)
					{
						int current = routes[r]->vertices[v];
						int edge = data.adj_indexes[last][current];
						red_cost += (pricing->getArtificials() ? 0 : data.edges[edge].cost) - duals.vertices[current];
						cost += data.edges[edge].cost;
						cerr << " " << current;
						if ((&bounds != nullptr) && (bounds.edges[edge] != -1))
							red_cost -= duals.bounds[bounds.edges[edge]];
						last = current;
					}
					cerr << endl << "Route(" << r << ") - RC = " << red_cost << " x " << routes[r]->red_cost << endl;
				}
				if (&bounds != nullptr)
				{
					for (int b = 0; b < (int)bounds.info.size(); b++)
					{
						const auto& bound = bounds.info[b];
						if (bound.key.type == FullEdge)
							cerr << "x(" << data.edges[bound.key.id].from << ")(" << data.edges[bound.key.id].to << ") = "
								<< bound.value.lb << " - rc = " << duals.bounds[b] << endl;
						else if (bound.key.type == Task)
							cerr << "y(" << bound.key.id << ") = [" << bound.value.lb << ", " << bound.value.ub << "] - dual = "
							<< duals.vertices[bound.key.id] << endl;
					}
				}
				exit(-1);
			}
			//DEBUG!
			found = true;
		}

		if (total_inserted > 0)
			return Master;
		else
		{
			depot = (depot + 1) % data.depots.size();
			if (depot == last_depot)
				if (stab_factor > EPS)
				{
					stab_factor -= 0.1;
					if (stab_factor <= EPS) stab_factor = 0.0;
					//last_duals = duals + (duals + last_duals) * -(stab_factor / (stab_factor + 0.1));
					last_duals.vertices = last_duals.vertices * stab_factor + duals.vertices * (1 - stab_factor);
					last_duals.bounds = last_duals.bounds * stab_factor + duals.bounds * (1 - stab_factor);
					stab_change = true;
					found = false;
				}
				else if (found)
				{
					out << "Duplicate route found!" << endl;
					throw;
				}
				else
				{
					if (solver.isCovering())
					{
						solver.goPartitioning();
						return Master;
					}
					return End;
				}
		}
	}
	if (solver.isCovering())
	{
		solver.goPartitioning();
		return Master;
	}
	return End;
}

void ColumnGeneration::enumerateRoutes()
{
	if (data.ub == numeric_limits<double>::infinity())
	{
		out << "No known upper bound. Aborting enumeration!" << endl;
		return;
	}
	out << "Enumerating for gap = " << (data.ub - solver.getObjValue()) / data.ub * 100 << "%:" << endl;

	RouteEnumeration enumeration;
	enumeration.setGap(data.ub - solver.getObjValue());

	SetPartitioning other;
	auto& duals = solver.getDuals();
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		out << "Enumerating depot " << d << "... ";

		//TODO: duals now have bounds!
		throw new runtime_error("Check implementation: Duals now have bounds!");
		backward->price(d, duals, solver.getBounds(), solver.hasArtificials());
		const auto& bw_cbs = backward->getCompletionBounds();

		enumeration.setCompletionBounds(bw_cbs);
		//TODO: duals now have bounds!
		enumeration.price(d, duals, solver.getBounds(), solver.hasArtificials());
		out << enumeration.getRoutes().size() << " routes found!" << endl;
		other.insertRoutes(enumeration.getRoutes());
	}

	other.goPartitioning();
	other.goInt();
	other.run();

	out << "Integral solution = " << other.getObjValue() << endl;
}

void ColumnGeneration::fixByReducedCost()
{
	auto& duals = solver.getDuals();

	vector<double> values(data.depots.size());
	vector<vector<double>> fw_cbs(data.depots.size());
	vector<vector<double>> bw_cbs(data.depots.size());

	double lagrangian = solver.getObjValue();

	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		//TODO: duals now have bounds!
		throw new runtime_error("Check implementation: duals now have bounds!");
		exact->price(d, duals, solver.getBounds(), solver.hasArtificials());
		backward->price(d, duals, solver.getBounds(), solver.hasArtificials());

		fw_cbs[d] = exact->getCompletionBounds();
		bw_cbs[d] = backward->getCompletionBounds();

		values[d] = exact->getValue();
		lagrangian += values[d] * data.depots[d].vehicles;
	}

	int total = 0;
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		double lb = lagrangian - values[d];
		for (int v = 0; v < (int)data.vertices.size(); v++)
		{
			//TODO: duals now have bounds!
			double bound = lb + fw_cbs[d][v] + bw_cbs[d][v] - duals.vertices[v];
			if (bound > data.ub)
				total++;
		}
	}
	out << "Fixed " << total << " vertices by red cost." << endl;
}

void ColumnGeneration::setRoot(bool value)
{
	is_root = value;
	setSilent(!is_root || params.silent);
	solver.setSilent(!is_root || params.silent);
	if (heuristic != nullptr) heuristic->setSilent(!is_root || params.silent);
	if (exact != nullptr) exact->setSilent(!is_root || params.silent);
}

void ColumnGeneration::dumpSolution(const vector<Route>& fixed_routes)
{
	static int index = 1;
	stringstream ss;
	ss << "out/" << data.name << "-" << index++ << ".txt";

	ofstream fout(ss.str(), ios::out);
	_dumpSolution(fout, fixed_routes);
	fout.close();
}

void ColumnGeneration::_dumpSolution(ostream& os, const vector<Route>& fixed_routes)
{
	vector<const Route*> solution;
	auto& sols = solver.getRouteSolution();
	auto& routes = solver.getRoutes();
	for (int d = 0; d < (int)data.depots.size(); d++)
	{
		os << "Depot " << d << ":" << endl;
		for (auto& route : sols[d])
		{
			solution.push_back(routes[d][route.first]);
			os << "Route " << route.first << " (" << route.second << "):";
			for (auto& vertex : routes[d][route.first]->vertices)
				os << " " << vertex;
			os << endl;
		}
	}

	os << "Fixed routes:" << endl;
	for (auto& route : fixed_routes)
	{
		solution.push_back(&route);
		os << "Depot " << route.depot << ", " "Route " << route.id << " (1):";
		for (auto& vertex : route.vertices)
			os << " " << vertex;
		os << endl;
	}

	SolutionChecker checker(os);
	checker.run(solution, solver.getObjValue());
}
