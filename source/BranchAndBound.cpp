#include "BranchAndBound.h"

#include <algorithm>

#include "Data.h"
#include "Parameters.h"
#include "Route.h"

constexpr auto FRAC = 0.5;
constexpr auto LIMIT = 1;

using namespace std;

BranchAndBound::BranchAndBound()
{
	setHandler(this);

	inf = numeric_limits<double>::infinity();
	lb = -inf;
	ub = (params.bb.dive ? inf : data.ub);
	ub_veh_num = inf;

	mip_value = numeric_limits<double>::infinity();
	root_value = -inf;
	root_time = mip_time = total_time = 0.0;

	total_nodes = 0;
	dive_cost = 0.0;
	dive_depth = 0;
	total_sd = 0;
}

bool BranchAndBound::run()
{
	timer.start();

	Node* root = new Node;
	root->chosen = -1;
	root->integral = false;
	for (int d = 0; d < (int)data.depots.size(); d++)
		root->bounds.info.emplace_back(VarKey(d, -1, Fleet), BoundValue(0, params.cg.relax_fleet ? numeric_limits<int>::max() : data.depots[d].vehicles));
	root->bounds.edges.resize(data.edges.size(), -1);

	solver.setRoot(true);
	if (solveNode(*root))
		queue.push(root);
	root_value = lb = root->value;
	root_time = timer.lap();
	solver.setRoot(false);

	while (!queue.empty())
	{
		const Node* node = queue.top();
		queue.pop();

		if (node->value - lb < -EPS)
		{
			cerr << "ERROR: New LB is lower than previous one! Aborting..." << endl;
			cerr << "node = " << node->value << ", LB = " << lb << endl;
			exit(-1);
		}

		lb = node->value;
		if (lb - ub > -EPS) break;

		if (!params.bb.dive) branch(*node);
		else dive(*node);

		delete node;
	}
	if ((lb - ub > EPS) || (queue.empty())) lb = ub;
	
	doCleanExit();

	return true;
}

bool BranchAndBound::solveNode(Node& node)
{
	char result = 'X';
	bool ok = true;

	solver.setBounds(node.bounds);
	if (params.bb.dive)
		solver.cleanModel();

	if (solver.run())
	{
		node.value = solver.getObjValue() + dive_cost;

		if (node.value - ub >= -EPS)
		{
			result = 'P';
			ok = false;
		}
		else
		{
			if (generateCandidates(node))
				result = 'F';
			else
			{
				node.integral = true;
				if (node.value - ub < -EPS)
				{
					ub = node.value;
					ub_veh_num = (int)dive_routes.size();
					auto& solution = solver.getRouteSolution();
					for (int d = 0; d < (int)data.depots.size(); d++)
						for (auto& route_sol : solution[d])
							ub_veh_num += route_sol.second;
				}
				result = 'I';
				solver.dumpSolution(dive_routes);
			}
		}
	}
	else
	{
		node.value = inf;
		result = 'X';
		ok = false;
	}

	if (node.chosen == -1) printHeader();
	printNode(node, result);

	total_nodes++;

	return ok;
}

void BranchAndBound::branch(const Node& node)
{
	Candidate cand = node.cands.front();
	Node* left = new Node(node);
	Node* right = new Node(node);

	int chosen = 0;
	if (cand.key.type == VarType::Fleet) chosen = cand.key.depot;
	else if (cand.key.type == VarType::FullEdge)
	{
		chosen = node.bounds.edges[cand.key.id];
		if (chosen == -1)
		{
			const Edge& edge = data.edges[cand.key.id];
			int ub = numeric_limits<int>::max();
			if ((data.vertices[edge.from].type == Vertex::Task) || (data.vertices[edge.to].type == Vertex::Task)) ub = 1;
			left->bounds.info.push_back(Bound(cand.key, { 0, ub }));
			right->bounds.info.push_back(Bound(cand.key, { 0, ub }));
			chosen = (int)node.bounds.info.size();
			left->bounds.edges[cand.key.id] = chosen;
			right->bounds.edges[cand.key.id] = chosen;
		}
	}
	else throw runtime_error("Cannot branch using this kind of variable!");

	if (left != nullptr)
	{
		left->chosen = chosen;
		left->bounds.info[chosen].value.ub = (int)(cand.value + EPS);
		if (solveNode(*left)) queue.push(left);
		else delete left;
	}

	if (right != nullptr)
	{
		right->chosen = chosen;
		right->bounds.info[chosen].value.lb = (int)(cand.value + 1.0 + EPS);
		if (solveNode(*right)) queue.push(right);
		else delete right;
	}
}

void BranchAndBound::dive(const Node& node)
{
	if ((!params.bb.strong_dive) || (dive_depth >= params.bb.strong_dive_max_depth)) regularDiving(node);
	else strongDiving(node);

	dive_depth++;
}

void BranchAndBound::regularDiving(const Node& node)
{
	Node* next = new Node(node);
	vector<bool> fixed(data.vertices.size(), false);

	int total = 0;
	for (int r = 0; r < (int)node.cands.size(); r++)
	{
		const auto& cand = node.cands[r];
		if (cand.key.type == VarType::Vehicle)
		{
			const Route& route = node.routes[r];

			bool infeasible = false;
			for (int v : route.vertices)
				if (fixed[v]) infeasible = true;
			if (infeasible) continue;

			for (int v : route.vertices)
			{
				if (data.vertices[v].type != Vertex::Task) continue;
				next->chosen = (int)next->bounds.info.size();
				next->bounds.info.push_back(Bound({ -1, v, VarType::Task }, { 0, 0 }));
				fixed[v] = true;
			}
			dive_cost += route.cost;
			dive_routes.push_back(route);

			if (++total == params.bb.dive_max_sol) break;
		}
		else throw runtime_error("Cannot dive using this kind of variable!");
	}

	if (solveNode(*next)) queue.push(next);
	else delete next;
}

void BranchAndBound::strongDiving(const Node& node)
{
	int total = 0;
	vector<pair<Node*, const Route*>> nodes;

	for (int r = 0; r < (int)node.cands.size(); r++)
	{
		const auto& cand = node.cands[r];
		Node* next = new Node(node);
		if (cand.key.type == VarType::Vehicle)
		{
			const Route& route = node.routes[r];
			for (int v : route.vertices)
			{
				if (data.vertices[v].type != Vertex::Task) continue;
				next->chosen = (int)next->bounds.info.size();
				next->bounds.info.push_back(Bound({ -1, v, VarType::Task }, { 0, 0 }));
			}
			nodes.emplace_back(next, &route);
		}
		else throw runtime_error("Cannot dive using this kind of variable!");

		total_sd++;
		if (++total >= params.bb.strong_dive_max_sol) break;
	}

	bool integral = false;
	pair<Node*, const Route*> min_elem = { nullptr, nullptr };
	double min_cost = numeric_limits<double>::infinity();
	for (auto& elem : nodes)
	{
		Node& cand_node = *elem.first;
		const Route& cand_route = *elem.second;

		dive_cost += cand_route.cost;
		dive_routes.push_back(cand_route);
		if ((solveNode(cand_node)) && (cand_node.value - min_cost < -EPS))
		{
			min_elem = elem;
			min_cost = cand_node.value;
			if (cand_node.integral) integral = true;
		}
		else delete elem.first;
		dive_cost -= cand_route.cost;
		dive_routes.pop_back();
	}

	if (min_elem.first != nullptr)
	{
		dive_cost += min_elem.second->cost;
		dive_routes.push_back(*min_elem.second);
		queue.push(min_elem.first);
	}
}

bool BranchAndBound::generateCandidates(Node& node)
{
	node.cands.clear();
	node.routes.clear();
	auto& routes = solver.getRoutes();
	auto& solution = solver.getSolution();
	for (const auto& elem : solution)
	{
		if ((params.bb.dive) && (elem.first.type != VarType::Vehicle)) continue;
		if ((!params.bb.dive) && (elem.first.type == VarType::Vehicle)) continue;
		
		double value = elem.second;
		double frac = value - (int)(value + EPS);
		if (frac > EPS)
			if (elem.first.type != VarType::Vehicle)
				node.cands.emplace_back(elem.first, value, fabs(FRAC - frac));
			else
				node.cands.emplace_back(elem.first, value, frac);
	}

	// First fleet vars, then task -> task, then the rest.
	sort(node.cands.begin(), node.cands.end(), [&](const Candidate& left, const Candidate& right)
	{
		if ((left.key.type == VarType::Fleet) && (right.key.type == VarType::FullEdge)) return true;
		if ((left.key.type == VarType::FullEdge) && (right.key.type == VarType::Fleet)) return false;
		if ((left.key.type == VarType::Fleet) && (right.key.type == VarType::Fleet)) return (left.frac - right.frac < -EPS);
		if ((left.key.type == VarType::Vehicle) && (right.key.type == VarType::Vehicle)) return (left.value - right.value >= EPS);

		const Edge& ledge = data.edges[left.key.id];
		const Edge& redge = data.edges[right.key.id];

		const Vertex& lfrom = data.vertices[ledge.from];
		const Vertex& lto = data.vertices[ledge.to];

		const Vertex& rfrom = data.vertices[redge.from];
		const Vertex& rto = data.vertices[redge.to];

		if ((lfrom.type == Vertex::Task) && (lto.type == Vertex::Task) && ((rfrom.type != Vertex::Task) || (rto.type != Vertex::Task))) return true;
		if ((rfrom.type == Vertex::Task) && (rto.type == Vertex::Task) && ((lfrom.type != Vertex::Task) || (lto.type != Vertex::Task))) return false;

		return (left.frac - right.frac < -EPS);
	});

	if (params.bb.dive)
	{
		int max_sol = max(params.bb.dive_max_sol, params.bb.strong_dive_max_sol);
		if (max_sol < (int)node.cands.size()) node.cands.resize(max_sol);
		for (int c = 0; c < (int)node.cands.size(); c++)
			node.routes.emplace_back(*routes[node.cands[c].key.depot][node.cands[c].key.id]);
	}

	return (!node.cands.empty());
}

void BranchAndBound::printHeader()
{
	out << endl
		<< setw(5) << "Node" << " "
		<< setw(5) << "Heur" << " "
		<< setw(5) << "Left" << " "
		<< setw(13) << "Objective" << " "
		<< setw(13) << "Lower Bound" << " "
		<< setw(13) << "Upper Bound" << " "
		<< setw(5) << "Gap" << " "
		<< setw(3) << " " << " "
		<< setw(22) << "Branch Variable" << " "
		<< endl;
}

void BranchAndBound::printNode(Node &node, char result)
{
	stringstream variable;
	if (node.chosen != -1)
	{
		auto& bound = node.bounds.info[node.chosen];
		if (bound.key.type == Fleet)
		{
			variable << "y(" << bound.key.depot << ") = [" << bound.value.lb << ", ";
			if (bound.value.ub == numeric_limits<int>::max()) variable << "inf";
			else variable << bound.value.ub;
			variable << "] FL";
		}
		else if (bound.key.type == FullEdge)
		{
			int from = data.edges[bound.key.id].from;
			int to = data.edges[bound.key.id].to;
			if ((data.vertices[from].type == Vertex::Task) || (data.vertices[to].type == Vertex::Task))
				variable << "x(" << from << ")(" << to << ") = " << bound.value.lb << " ED";
			else
			{
				variable << "x(" << from << ")(" << to << ") = [" << bound.value.lb << ", ";
				if (bound.value.ub == numeric_limits<int>::max()) variable << "inf";
				else variable << bound.value.ub;
				variable << "] ED";
			}
		}
		else if (bound.key.type == Task)
			variable << " DIVE";
	}

	out << setprecision(1) << fixed
		<< setw(5) << total_nodes << " "
		<< setw(5) << total_sd << " "
		<< setw(5) << queue.size() << " "
		<< setw(13) << node.value << " "
		<< setw(13) << lb << " "
		<< setw(13) << ub << " "
		<< setw(5) << (ub != inf ? (ub - lb) / (double)ub * 100 : 100) << "%"
		<< setw(3) << result << " "
		<< setw(22) << variable.str() << " " << endl;
}

void BranchAndBound::doCleanExit()
{
	total_time = timer.stop();

	cout << setprecision(3) << fixed << "BaP," << data.name << "," << data.num_depots << "," << data.num_stations << "," << data.num_tasks;
	cout << "," << root_value << "," << root_time << ","
		<< mip_value << "," << mip_time << "," << lb << "," << ub << "," << total_time << "," << total_nodes << "," << ub_veh_num;
	if (params.bb.dive)
	{
		cout << "," << dive_depth << "," << params.bb.dive_max_sol;
		if (params.bb.strong_dive)
			cout << "," << total_sd << "," << params.bb.strong_dive_max_sol << "," << params.bb.strong_dive_max_depth;
		else
			cout << ",0,0,0";
	}
	cout << endl;
}
