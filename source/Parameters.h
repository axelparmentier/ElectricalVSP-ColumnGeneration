#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "Singleton.h"

#include <string>

class Parameters : public Singleton<Parameters>
{
	friend class Singleton<Parameters>;
public:
	bool load(int argc, char** argv);

	enum ProblemType
	{
		VSP,
		EVSP,
		NoProblem,
	};

	bool isThereVehicleWithInitialCharge;
	bool isSequencesOfStationsAuthorized;

	int maxNumberOfDepots;
	int maxNumberOfStations;
	int maxNumberOfTasks;

	enum AlgorithmType
	{
		ColumnGeneration,
		BranchAndPrice,
		SolutionFileChecker,
		SimplePricingRunner,
		NoAlgorithm,
	};

	enum PricingType
	{
		Exact,
		Monoid,
	};

	enum BoundsType
	{
		Single,
		Clustered,
		Conditional,
		SingleThenConditional
	};

	enum SpdLogLevel
	{
		trace,
		debug,
		info,
		warn,
		err,
		critical,
		off,
	};

	std::string file_name;
	AlgorithmType algo_type;
	ProblemType prob_type;

	bool silent;
	int time_limit;

	struct CGParams
	{
		bool mip;
		bool sp;
		bool fix_fleet;
		bool relax_fleet;
		bool no_clean;
		double clean_factor;
		PricingType pricing;
		BoundsType bounds;
		int label_map_max_size;
		bool isLogToConsole;
		std::string logFileName;
		SpdLogLevel spdLogLevel;
		int max_routes;
		bool isPositiveReducedCostRouteKept;
		bool discardUsingDominance;
		int memoryBoundedMaxSize;
		bool no_singleton_routes;
	} cg;

	struct BBParams
	{
		bool dive;
		bool strong_dive;
		int dive_max_sol;
		int strong_dive_max_sol;
		int strong_dive_max_depth;
	} bb;

	struct ClusterBoundsParam{
		int nbClustersPerVertex;
	} clus;

	struct ConditionalBoundsParam
	{
		double delta;
		double ubCost;

	} cond;

	bool isGraphSparsified;
	struct SparsifyParam{
		double noChargeDistanceCoef;
		double noChargeTimeCoef;
		double noChargeIncompressibleLostTimeCoef;
		int noChargeNbArcsKept;
		int noChargeNbArcsDepotKept;
		double chargeDistanceCoef;
		double chargeTimeCoef;
		double chargeIncompressibleLostTimeCoef;
		// double chargeAdditionalChargeCoef;
		int chargeNbArcsKept;
		int chargeNbArcsDepotKept;
	} spar;

	struct BatchPricingParam
	{
		int nbClusterByPartitions;
		int nbPartitions;
		int nbBatches;
	} batch;

	struct SPRParams
	{
		int index;
	} simple;

private:
	Parameters();

	bool help(const std::string& bad_option = "");
};

#endif
