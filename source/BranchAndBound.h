#ifndef BRANCHANDBOUND_H
#define BRANCHANDBOUND_H

#include "Algorithm.h"
#include "SignalHandler.h"

#include <vector>
#include <queue>

#include "ColumnGeneration.h"
#include "BranchAndPriceStructs.h"

#include "Timer.h"
#include "Templates.h"

class BranchAndBound : public Algorithm, public SignalHandler
{
public:
	BranchAndBound();

	bool run() override;

	void doCleanExit() override;

private:
	class Candidate
	{
	public:
		Candidate() : value(0.0), frac(0.0) {}
		Candidate(const VarKey& key, double value, double frac) : key(key), value(value), frac(frac) {}
		Candidate(const VarKey&& key, double value, double frac) : key(key), value(value), frac(frac) {}

		VarKey key;
		double frac;
		double value;

		bool operator<(const Candidate& right) const { return (key.type == right.key.type ? frac < right.frac : key.type < right.key.type); }
	};

	class Node
	{
	public:
		double value;
		
		int chosen;
		Bounds bounds;
		std::vector<Candidate> cands;
		std::vector<Route> routes;
		bool integral;

		bool operator<(const Node& right) const { return value < right.value; }
		bool operator>(const Node& right) const { return value > right.value; }
	};

	double lb;
	double ub;
	double inf;

	double ub_veh_num;

	double dive_cost;
	int dive_depth;
	std::vector<Route> dive_routes;

	double root_value;
	double root_time;

	double mip_value;
	double mip_time;

	double total_time;
	int total_nodes;
	int total_sd;

	Timer timer;

	ColumnGeneration solver;
	std::priority_queue<Node*, std::vector<Node*>, Greater<Node*>> queue;

	bool solveNode(Node& node);
	void branch(const Node& node);
	void dive(const Node& node);
	void regularDiving(const Node& node);
	void strongDiving(const Node& node);
	bool generateCandidates(Node& node);

	void printHeader();
	void printNode(Node &node, char result);
};

#endif
