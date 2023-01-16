/*
 * MonoidPricing.h
 *
 *  Created on: Aug 24, 2018
 *      Author: axel
 */

#ifndef SOURCE_MONOIDPRICING_H_
#define SOURCE_MONOIDPRICING_H_

#include "../latticeRCSP/mrcsp/MRCSPproblem.h"
#include "ResourceEvsp.h"

#include <vector>

#include "Pricing.h"
#include "Data.h"

#include "../latticeRCSP/latticeRCSP/Tools/Console.h"

#define MULTI_CHARGE

using namespace mrcsp;
using namespace std;

class MonoidPricing : public Pricing {
public:
	MonoidPricing(bool approximatePricer_l = false);

	void init();
	bool do_price() override;

	void buildCompletionBounds() override;

private:
	int getDepotBatchProblem(int depot, int batch);
	void buildBatches();

	int buildProblems();
	int buildProblem(int depotId, int batch_l);
	void updateProblem(int depotId, int batch_l);
	int getDestinationId() const;
	int getEdgeIdWithCharge(int edgeId, int stationId) const;

	int buildStationArcs();
	void initStations();
	int buildStationArcsExact();
	int buildStationArcsSparsified();
	int sparsifyBatchesStationArcs();

	bool priceBatch(int batch_l);

	void setPriceAllRoutesSmallerThanGap(bool b){priceAllRoutesSmallerThanGap = b;}
	void setGap(double g){gap = g;}

	using EvspProblem = MRCSPproblem<ResourceEvsp, ResourceEvspFunctions>;



#ifdef MULTI_CHARGE
	struct StationArc {
		int id;
		int depotId; // -1 if neither tail nor head is a depot.
		int from;
		int to;
		double cost;
		ResourceEvsp resourceStandard;
		ResourceEvsp resourceArtificial;
		vector<int> vertices;
	};
#elif
	// struct StationArc {
	// 	string id;
	// 	int station;
	// 	double charge;
	// 	double fromToStaCost;
	// 	double staToToCost;
	// };
#endif
	struct ProblemWithInfo
	{
		int depot;
		int vertexBatch;
		bool previousArtificials;
		bool previousOptimal;
		double previousValue;
		EvspProblem problem;
	};
	bool isInitialized;
	vector<StationArc> stationArcs;
	vector<ProblemWithInfo> problems;
	vector<int> currentBatchOfDepot;
	vector<vector<int> > verticesOfBatch;
	vector<vector<StationArc *> > stationArcsOfBatch;

	vector<int> stations;
	// vector<bool> previousArtificials;
	// vector<bool> previousOptimal;
	// vector<double> previousValue;
	level::level_enum level_m;

	// std::vector<std::vector<int>> arc_mapping; replaced by following
	void buildStationArcsOfBatchArc_mapping();
	vector<vector<vector< StationArc *> > > stationArcsOfBatchArc_mapping;

	bool priceAllRoutesSmallerThanGap;
	double gap;

	bool approximatePricer;
};

#endif /* SOURCE_MONOIDPRICING_H_ */
