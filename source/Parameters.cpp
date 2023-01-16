#include "Parameters.h"

#include <iostream>
#include <limits>

using namespace std;

Parameters::Parameters()
{
	algo_type = NoAlgorithm;
	prob_type = EVSP;
	time_limit = 21600;
	silent = false;

	isThereVehicleWithInitialCharge = false;
	isSequencesOfStationsAuthorized = false;

	maxNumberOfDepots = numeric_limits<int>::max();
	maxNumberOfStations = numeric_limits<int>::max();
	maxNumberOfTasks = numeric_limits<int>::max();

	cg.mip = false;
	cg.sp = false;
	cg.fix_fleet = false;
	cg.relax_fleet = false;
	cg.no_clean = false;
	cg.clean_factor = 1.0;
	cg.pricing = Exact;
	cg.bounds = SingleThenConditional;
	cg.label_map_max_size = 20000000;
	cg.isLogToConsole = false;
	cg.logFileName = "MRCSPlog.txt";
	cg.spdLogLevel = off;
	cg.max_routes = 200;
	cg.isPositiveReducedCostRouteKept = false;
	cg.discardUsingDominance = true;
	cg.memoryBoundedMaxSize = 10000;
	cg.no_singleton_routes = false;

	bb.dive = false;
	bb.strong_dive = false;
	bb.dive_max_sol = 1;
	bb.strong_dive_max_sol = 2;
	bb.strong_dive_max_depth = numeric_limits<int>::max();

	clus.nbClustersPerVertex = 20;

	cond.delta = 10.0;
	cond.ubCost = 0.0;

	isGraphSparsified = false;
	spar.noChargeDistanceCoef = 1.0;
	spar.noChargeTimeCoef = 0.2; // 0.2 was the best without incompressible lost time
	spar.noChargeIncompressibleLostTimeCoef = 0.0;
	spar.noChargeNbArcsKept = 15;
	spar.noChargeNbArcsDepotKept = 2;
	spar.chargeDistanceCoef = 1.0;
	spar.chargeTimeCoef = 0.1; // 0.2 was the best without incompressible lost time
	spar.chargeIncompressibleLostTimeCoef = 0.1;
	// spar.chargeAdditionalChargeCoef = 0.0;
	spar.chargeNbArcsKept = 15;
	spar.chargeNbArcsDepotKept = 2;

	batch.nbClusterByPartitions = 1;
	batch.nbPartitions = 1;
	batch.nbBatches = 1;

	simple.index = -1;
}

bool Parameters::load(int argc, char** argv)
{
	if (argc < 3) return help();

	file_name = string(argv[1]);

	for (int i = 2; i < argc; i++)
	{
		string option(argv[i]);
		if (option == "-cg") algo_type = ColumnGeneration;
		else if (option == "-bp") algo_type = BranchAndPrice;
		else if (option == "-sc") algo_type = SolutionFileChecker;
		else if (option == "-sp") algo_type = SimplePricingRunner;

		else if (option == "-vsp") prob_type = VSP;
		else if (option == "-evsp") prob_type = EVSP;

		else if (option == "-veh") isThereVehicleWithInitialCharge = true;
		else if (option == "-seq") isSequencesOfStationsAuthorized = true;

		else if (option == "-mnd") maxNumberOfDepots = atoi(argv[++i]);
		else if (option == "-mns") maxNumberOfStations = atoi(argv[++i]);
		else if (option == "-mnt") maxNumberOfTasks = atoi(argv[++i]);

		else if (option == "-tl") time_limit = atoi(argv[++i]);
		else if (option == "-s") silent = true;

		else if (option == "-sp-id") simple.index = atoi(argv[++i]);

		else if (option == "-cg-mip") cg.mip = true;
		else if (option == "-cg-sp") cg.sp = true;
		else if (option == "-cg-ff") cg.fix_fleet = true;
		else if (option == "-cg-rf") cg.relax_fleet = true;
		else if (option == "-cg-nc") cg.no_clean = true;
		else if (option == "-cg-cf") cg.clean_factor = atof(argv[++i]);
		else if (option == "-cg-mr") cg.max_routes = atoi(argv[++i]);
		else if (option == "-cg-pr")
		{
			string pricing(argv[++i]);
			if (pricing == "ex") cg.pricing = Exact;
			else if (pricing == "mo") cg.pricing = Monoid;
		}
		else if (option == "-cg-bo")
		{
			string bounds(argv[++i]);
			if (bounds == "si") cg.bounds = Single;
			else if (bounds == "cl") cg.bounds = Clustered;
			else if (bounds == "co") cg.bounds = Conditional;
			else if (bounds == "sc") cg.bounds = SingleThenConditional;
		}
		else if (option == "-cg-lm")
		{
			cg.label_map_max_size = atoi(argv[++i]);
		}
		else if (option == "-cg-logConsole") {
			cg.isLogToConsole = true;
		}
		else if (option == "-cg-logFile")
		{
			cg.logFileName = argv[++i];
		}
		else if (option == "-cg-logLevel")
		{
			string level = argv[++i];
			if (level == "trace") cg.spdLogLevel = trace;
			else if (level == "debug") cg.spdLogLevel = debug;
			else if (level == "info") cg.spdLogLevel = info;
			else if (level == "warn") cg.spdLogLevel = warn;
			else if (level == "err") cg.spdLogLevel = err;
			else if (level == "critical") cg.spdLogLevel = critical;
			else if (level == "off") cg.spdLogLevel = off;
		}
		else if (option == "-cg-pk") cg.isPositiveReducedCostRouteKept = true;
		else if (option == "-cg-nd") cg.discardUsingDominance = false;
		else if (option == "-cg-memBMax") cg.memoryBoundedMaxSize = atoi(argv[++i]);
		else if (option == "-cg-nsr") cg.no_singleton_routes = true;

		else if (option == "-bb-d") bb.dive = true;
		else if (option == "-bb-sd") bb.strong_dive = true;
		else if (option == "-bb-dms") bb.dive_max_sol = atoi(argv[++i]);
		else if (option == "-bb-sdms") bb.strong_dive_max_sol = atoi(argv[++i]);
		else if (option == "-bb-sdmd") bb.strong_dive_max_depth = atoi(argv[++i]);

		else if (option == "-clus-nb") {
			clus.nbClustersPerVertex = atoi(argv[++i]);
		}
		else if (option == "-cond-delta") {
			cond.delta = atof(argv[++i]);
		}
		else if (option == "-spar") {
			isGraphSparsified = true;
		}
		else if (option == "-spar-nd") {
			spar.noChargeDistanceCoef = atof(argv[++i]);
		}
		else if (option == "-spar-nt") {
			spar.noChargeTimeCoef = atof(argv[++i]);
		}
		else if (option == "-spar-ni") {
			spar.noChargeIncompressibleLostTimeCoef = atof(argv[++i]);
		}
		else if (option == "-spar-nn") {
			spar.noChargeNbArcsKept = atoi(argv[++i]);
		}
		else if (option == "-spar-nnd") {
			spar.noChargeNbArcsDepotKept = atoi(argv[++i]);
		}
		else if (option == "-spar-cd") {
			spar.chargeDistanceCoef = atof(argv[++i]);
		}
		else if (option == "-spar-ct") {
			spar.chargeTimeCoef = atof(argv[++i]);
		}
		else if (option == "-spar-ci") {
			spar.chargeIncompressibleLostTimeCoef = atof(argv[++i]);
		}
		// else if (option == "-spar-ca"){
		// 	spar.chargeAdditionalChargeCoef = atof(argv[++i]);
		// }
		else if (option == "-spar-cn") {
			spar.chargeNbArcsKept = atoi(argv[++i]);
		}
		else if (option == "-spar-cnd") {
			spar.chargeNbArcsDepotKept = atoi(argv[++i]);
		}
		else if (option == "-batch-nbc") {
			batch.nbClusterByPartitions = atoi(argv[++i]);
		}
		else if (option == "-batch-nbp") {
			batch.nbPartitions = atoi(argv[++i]);
		}
		//		else if (option == "-cond-ub"){
		//			cond.ubCost = atof(argv[++i]);
		//		}
		else
		{
			help(option);
			return false;
		}
	}

	if ((batch.nbPartitions > 1) && (batch.nbClusterByPartitions == 1))
	{
		cerr << "error: several partitions with one cluster" << endl;
		return false;
	}
	batch.nbBatches = batch.nbClusterByPartitions * batch.nbPartitions;

	return true;
}

bool Parameters::help(const string& bad_option)
{
	if (!bad_option.empty()) cout << "Invalid option: " << bad_option << endl;

	cout << "Usage: mdvsptw <instance-file> [options]" << endl;
	cout << "Problem Types: " << endl;
	cout << "  -evsp: Electric Vehicle Scheduling Problem (default)." << endl;
	cout << "  -vsp: Regular Vehicle Scheduling Problem (it probably won't work)." << endl;
	cout << "Problem variant: " << endl;
	cout << "  -veh: vehicles with initial position." << endl;
	cout << "  -seq: authorizes sequences of stations (default = not authorized)." << endl;
	cout << "  -mnt <int>: Max number of tasks (default = unlimited)." << endl;
	cout << "Algorithms: " << endl;
	cout << "  -cg: Column Generation." << endl;
	cout << "  -bp: Branch-and-Price." << endl;
	cout << "  -sc: Solution File Checker." << endl;
	cout << "  -sp: Simple Pricing Runner." << endl;
	cout << "Options: " << endl;
	cout << "  -tl <int>: Time limit (default = 21600)." << endl;
	cout << "  -s: Silent mode." << endl;
	cout << "Column Generation: " << endl;
	cout << "  -cg-pr <ex | mo>: Pricing algorithm (exact or monoid - default = exact)." << endl;
	cout << "  -cg-bo <si | cl | co | sc>: Bounds used by monoid pricing algorithm (single, clustered, conditional, or single then conditional - default = single then conditional)" << endl;
	cout << "  -cg-lm <int>: Label map max size (default = 10000000)." << endl;
	cout << "  -cg-mip: Run integer formulation after the column generation." << endl;
	cout << "  -cg-sp: Run the master as a set partitioning instead of covering." << endl;
	cout << "  -cg-ff: Fix fleet value." << endl;
	cout << "  -cg-rf: Relax fleet." << endl;
	cout << "  -cg-nc: Do not clean the set partitioning formulation." << endl;
	cout << "  -cg-cf <double>: Set partitioning clean factor. Percentage of column to keep besides the ones with null reduced costs. Default: 1.0" << endl;
	cout << "  -cg-logLevel <trace | debug | info | warn | err | critical | off>: set log level (default = off)" << endl;
	cout << "  -cg-logConsole: send MRCSP log to console (default = false, i.e., log to file)" << endl;
	cout << "  -cg-logFile: (default = MRCSPlog.txt)" << endl;
	cout << "  -cg-mr <int>: Maximum number of routes generated on each pricing (default = 200)." << endl;
	cout << "  -cg-no-cycle: Do not cycle throught vehicles. Generates all routes for a given vehicle before changing to the next one." << endl;
	cout << "  -cg-nd: do not discard path using dominance in pricing (default = false, i.e., dominance is used by default)" << endl;
	cout << "  -cg-pk: keep positive reduced cost routes priced (default =false)." << endl;
	cout << "  -cg-memBMax: path list max size in enumeration algorithm in approximate MonoidPricing (default = 10,000)" << endl;
	cout << "  -cg-nsr: No singleton routes (routes with one task to force feasibility)." << endl;
	cout << "Column Generation clustering pricer: " << endl;
	cout << "  -clus-nb <int>: number of states per vertex (default = 20)." << endl;
	cout << "Column Generation conditional pricer: " << endl;
	cout << "  -cond-delta <double>: maximum value delta between lower and upper bound of a cluster (default = 1)." << endl;
	cout << "Sparsifying the graph: " << endl;
	cout << "  -spar: activates graph sparsification " << endl;
	cout << "  -spar-nd <double>: no charge arcs distance coef (default = 1.0)" << endl;
	cout << "  -spar-nt <double>: no charge arcs time coef (default = 0.1)" << endl;
	cout << "  -spar-ni <double>: no charge arcs incompressible time coef (default = 0.1)" << endl;
	cout << "  -spar-nn <int>: nb no charge arcs kept per vertex (default = 15) " << endl;
	cout << "  -spar-nnd <int>: nb no charge arcs from and to depot kept per vertex (default = 1)" << endl;
	cout << "  -spar-cd <double>: charge arcs distance coef (default = 1.0)" << endl;
	cout << "  -spar-ct <double>: charge arcs time coef (default = 0.1)" << endl;
	cout << "  -spar-ci <double>: charge arcs incompresible time coef (default = 0.1)" << endl;
	// cout << "  -spar-ca <double>: charge arcs additional charge coef (default = 0.0)" << endl;
	cout << "  -spar-cn <int>: nb charge arcs kept per vertex (default = 15)" << endl;
	cout << "  -spar-cnd <int>: nb charge arcs from and to depot kept per vertex (default = 1)" << endl;
	//	cout << "  -cond-ub: upper bound on priced variable cost (default = 0)." << endl;
	cout << "Batching pricing (approximate):" << endl;
	cout << "  -batch-nbc <int> numberClusterByParititions (default = 1) Pricing exact if = 1" << endl;
	cout << "  -batch-nbp <int> numberPartitions (default = 1)" << endl;
	cout << "Branch-and-Bound:" << endl;
	cout << "  -bb-d: Perform a dive in the B&B tree to obtain a primal heuristic solution." << endl;
	cout << "  -bb-sd: Use strong diving. -bb-d must be activated." << endl;
	cout << "  -bb-dms <int>: Diving: Max number of solutions to fix on each iteration (default = 1)." << endl;
	cout << "  -bb-sdms <int>: Strong diving: max number of solutions to test on each iteration (default = 2)." << endl;
	cout << "  -bb-sdmd <int>: Strong diving: max depth to use (default = inf). After that, use regular diving." << endl;
	cout << "Simple Pricing Runner:" << endl;
	cout << "  -sp-id <int>: File index (eg: for D2_S4_C100_01-1-dual.txt, it should be 1). File should be in <out> folder." << endl;
	cout << endl;

	return false;
}
