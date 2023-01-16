/*
 * MRCSPproblem.h
 *
 *  Created on: 23 mars 2017
 *      Author: axelparmentier
 */

#ifndef SRC_MRCSP_MRCSPPROBLEM_H_
#define SRC_MRCSP_MRCSPPROBLEM_H_

#ifndef EPS
#define EPS 1e-6
#endif

#include<map>
#include<set>
#include "MRCSParc.h"
#include "MRCSPvertex.h"
#include "MRCSPfunctors.h"

#include "../latticeRCSP/GraphBuilder/ContextBuilder.h"
#include "../latticeRCSP/Context/ProblemRCSP.h"
#include "../latticeRCSP/Graph/GraphWithBounds.h"
#include "../latticeRCSP/Graph/GraphWithStateGraphBounds.h"
#include "../latticeRCSP/Graph/GraphWithConditionalBounds.h"
#include "../latticeRCSP/Algorithm/LabelSettingAlgorithm.h"
#include "../latticeRCSP/Algorithm/LabelCorrectingAlgorithm.h"
#include "../latticeRCSP/Algorithm/LabelDominanceAlgorithm.h"


using namespace std;
namespace mrcsp {

/*
 * CostConstraintFunctions must contain three methods
 *   - static double getCost(Resource const &)
 *   - static bool isConstraintSatisfied(Resource const &)
 *   - static double getResourceConstraintHeuristic(Resource const &) => used in heuristcs
 *       searching good candidates to evaluate the size of the resource (a smaller
 *       means a better candidate)
 *
 */

template <typename Resource, typename CostConstaintFunctions>
class MRCSPproblem
{
public:
   typedef MRCSPvertex<Resource> VertexPb;
   typedef MRCSParc<Resource> ArcPb;
   typedef MRCSPcostFunctor<Resource, CostConstaintFunctions> CostPb;
   typedef MRCSPconstraintFeasibilityFunctor<Resource, CostConstaintFunctions> ConstraintFeasibilityPb;
   typedef MRCSPconstraintHeuristicFunctor<Resource, CostConstaintFunctions> ConstraintHeuristicPb;
   typedef ProblemRCSP<Resource, CostPb, ConstraintFeasibilityPb, ConstraintHeuristicPb> Pb;

   enum class LabelAlgorithm {
      LabelSetting_withLabelMapMaxSize,
      LabelCorrecting_withLabelMapMaxSize,
      LabelDominance_withLabelMapMaxSize
   };

   enum class Bounds {
	   Single_Bound,
	   Clustered_Bounds,
	   Conditional_Bounds,
   };

   struct ClusterParameters {
	   double clusteringSimulatedAnnealingPeriod_s;
	   double clusteringSimulatedAnnealingMinImprovePerPeriod_percent;
	   double clusertingSimulatedAnnealingMaxTime_s;
	   int nbClustersPerVertex;
   };

   struct ConditionalParameters{
	   double ubCost;
	   double delta;
   };

private:
   static string arcSeparator;
   static string dummyOriginArcID;
   static string dummyDestinationArcID;

   set<string> originVertices;
   map<string, ArcContext<Resource> *> destinationVertices;
   map<string, VertexPb > vertices_m;
   map<string, ArcPb > arcs_m;

   struct ModificationsIndices {
        unsigned long resources;
        unsigned long structure;
        ModificationsIndices() {resources = 0; structure = 0;}
        void operator++(){++resources; ++structure;}
        bool operator==(const ModificationsIndices & i){return (resources == i.resources) && (structure ==i.structure);}
   };
   ModificationsIndices modificationIndices_m;

   //-------------
   // Parameters
   //-------------
   int nSolutionsSearched;
   bool isVertexResourceActivated;
   Bounds bounds_m;
   bool recomputeStateGraphIfResourceModification;
   LabelAlgorithm algorithm_m;
   unsigned int labelMapMaxSize;

   bool isMemoryBounded;
   bool isMemoryBoundedExact;
   int memoryBoundedMaxSize;


   //-------------
   // Resolution
   //-------------
//   map<double, list<string> > solution;
   Pb * pb_m;
   ModificationsIndices modificationsPb;

   ContextGraph<Resource> * contextGraph_m;
   ModificationsIndices modificationsContextGraph;
   GraphWithBounds<Resource, Pb> * graphWithBounds_m;
   ModificationsIndices modificationsGraphWithBounds;

//   unsigned int nClustersStatesPerVertex_m;
   ClusterParameters clusterParameters_m;
   GraphWithStateGraphBounds<Resource, Pb> * graphWithStateGraphBounds_m;
   ModificationsIndices modificationsGraphWithStateGraphBounds;

   ConditionalParameters conditionalParameters_m;
   GraphWithConditionalBounds<Resource, Pb, CostPb> * graphWithConditionalBounds_m;
   ModificationsIndices modificationsGraphWithConditionalBounds;

   //-------------
   // Approx res
   //-------------
   bool isStopBoundActivated_m;
   double stopBound_m;
   LabelSettingAlgorithmResults labelResult;
   bool approximatePricing_m;

public:
   MRCSPproblem();
   MRCSPproblem(const MRCSPproblem<Resource, CostConstaintFunctions> & pb);

   virtual ~MRCSPproblem();

   void addVertex(MRCSPvertexInfo<Resource>);
   void deleteVertex(string);
   void updateVertexResource(string, Resource);

   void addArc(MRCSParcInfo<Resource>);
   void deleteArc(string);
   void updateArcResource(string, Resource);
   size_t getNumArcs() { return arcs_m.size(); }

   void addOrigin(string);
   void deleteOrigin(string);
   void addDestination(string);
   void deleteDestination(string);


   //------
   void solve();

   //------
   void setIsVertexResourceActivated(bool b) {isVertexResourceActivated = b;}
   void setNSolutionsSearched(int);
   void setLabelAlgorithm(LabelAlgorithm algo){algorithm_m = algo;}
   void setBoundsType(Bounds b){bounds_m = b;}
//   void setNClustersStatesPerVertex(int n){nClustersStatesPerVertex_m = n;}
   void reinitializeParametersToDefault();
//   ContextGraph<Resource> * getContextGraph_rebuildItIfNeeded();
   multimap<double, pair<list<string>, Resource > > getSolutionsArcList();
   multimap<double, pair<list<string>, Resource > > getSolutionsVertexList();

   Resource rebuildPathResource(list<string> const & arcsList);
   Resource rebuildPathResourceFromEnd(list<string> const & arcsList);

	void setRecomputeStateGraphIfResourceModification(bool recomputeStateGraphIfResourceModification) {
		this->recomputeStateGraphIfResourceModification = recomputeStateGraphIfResourceModification;}

	void setLabelMapMaxSize(unsigned int s){labelMapMaxSize = s;}

	void setClusteringSimulatedAnnealingMaxTimeS(double clusertingSimulatedAnnealingMaxTimeS) {
		clusterParameters_m.clusertingSimulatedAnnealingMaxTime_s = clusertingSimulatedAnnealingMaxTimeS;}
	void setClusteringSimulatedAnnealingMinImprovePerPeriodPercent(double clusteringSimulatedAnnealingMinImprovePerPeriodPercent) {
		clusterParameters_m.clusteringSimulatedAnnealingMinImprovePerPeriod_percent =clusteringSimulatedAnnealingMinImprovePerPeriodPercent;}
	void setClusteringSimulatedAnnealingPeriodS(double clusteringSimulatedAnnealingPeriodS) {
		clusterParameters_m.clusteringSimulatedAnnealingPeriod_s = clusteringSimulatedAnnealingPeriodS;}
	void setNbClustersPerVertex(int nbClustersPerVertex) {
		clusterParameters_m.nbClustersPerVertex = nbClustersPerVertex;}

	void setConditionalUbCost(double ub){conditionalParameters_m.ubCost = ub;}
	void setConditionalDelta(double delta){conditionalParameters_m.delta = delta;}

   void setIsMemoryBounded(bool b){isMemoryBounded = b;}
   void setMemoryBoundedMaxSize(int s){memoryBoundedMaxSize = s;}
   bool getIsMemoryBoundedExact() const {return isMemoryBoundedExact;}

   //------ Debug
   void printGraphUsedByAlgorithmsToFile(string const & file);
   Resource getResourceOfArcUsedByAlgorithm(string const & arc);

   //StopBound
   void setIsStopBoundActivated(bool isStopBoundActivated){isStopBoundActivated_m = isStopBoundActivated;}
   void setStopBound(double stopBound){stopBound_m = stopBound;}
   LabelSettingAlgorithmResults getLabelResult() const {return labelResult;}
   void setApproximatePricing(bool);

private:
   void updateContextGraphIfNeeded();
   void updateProblemIfNeeded();
   void updateGraphWithBoundsIfNeeded();
   void updateGraphWithStateGraphBoundsIfNeeded();
   void computeGraphWithStateGraphBounds();
   void updateGraphWithConditionalBoundsIfNeeded();


   void buildContextGraph();
   void updateContextArcResources();
   list<string> parseSolutionToArcList(string const &);
   list<string> parseSolutionToEmptyPathSingleVertex(string const &);
   list<string> convertArcListToVertexList(list<string> const &);
   void reinitializeArcContextPointers();
   void setGraphAndPbPointersToNull();
   Resource computeArcResource(string const &);




};

} /* namespace mrcsp */

#include "MRCSPproblem.hpp"

#endif /* SRC_MRCSP_MRCSPPROBLEM_H_ */
