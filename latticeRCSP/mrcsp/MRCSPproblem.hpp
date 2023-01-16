/*
 * MRCSPproblem.cpp
 *
 *  Created on: 23 mars 2017
 *      Author: axelparmentier
 */

#include "MRCSPproblem.h"
#include <iostream>
#include <cassert>

#include "../latticeRCSP/Graph/Path.h"
#include "../latticeRCSP/Tools/Console.h"

namespace mrcsp {

template <typename Resource, typename CostConstaintFunctions>
string MRCSPproblem<Resource, CostConstaintFunctions>::arcSeparator = "_";

template <typename Resource, typename CostConstaintFunctions>
string MRCSPproblem<Resource, CostConstaintFunctions>::dummyOriginArcID = "dummyOriA_";

template <typename Resource, typename CostConstaintFunctions>
string MRCSPproblem<Resource, CostConstaintFunctions>::dummyDestinationArcID = "dummyDesA";

template <typename Resource, typename CostConstaintFunctions>
MRCSPproblem<Resource, CostConstaintFunctions>::MRCSPproblem()
: labelMapMaxSize(5000000)
{
   setGraphAndPbPointersToNull();
   reinitializeParametersToDefault();
}

template <typename Resource, typename CostConstaintFunctions>
MRCSPproblem<Resource, CostConstaintFunctions>::MRCSPproblem(const MRCSPproblem<Resource, CostConstaintFunctions> & pb)
:
   originVertices(pb.originVertices),
   destinationVertices(pb.destinationVertices),// getDestinationVertices();
   vertices_m(pb.vertices_m),// .getVertices();
   arcs_m(pb.arcs_m), //getArcs();
   nSolutionsSearched(pb.nSolutionsSearched),// getNSolutionsSearched();
   isVertexResourceActivated(pb.isVertexResourceActivated),// getIsVertexResourceActivated();
   bounds_m(pb.bounds_m),
   algorithm_m(pb.algorithm_m),
   clusterParameters_m(pb.clusterParameters_m),
   labelMapMaxSize(5000000),
   isStopBoundActivated_m(pb.isStopBoundActivated_m),
   stopBound_m(pb.stopBound_m),
   isMemoryBounded(pb.isMemoryBounded),
   isMemoryBoundedExact(pb.isMemoryBoundedExact),
   memoryBoundedMaxSize(pb.memoryBoundedMaxSize)
{
   arcSeparator = pb.arcSeparator;

   reinitializeArcContextPointers();
   setGraphAndPbPointersToNull();
}

template <typename Resource, typename CostConstaintFunctions>
MRCSPproblem<Resource, CostConstaintFunctions>::~MRCSPproblem()
{
   // TODO Auto-generated destructor stub
   if (graphWithBounds_m != NULL) delete graphWithBounds_m;
   if (pb_m != NULL) delete pb_m;
   if (contextGraph_m != NULL) delete contextGraph_m;
   if (graphWithStateGraphBounds_m != NULL) delete graphWithStateGraphBounds_m;
   if (graphWithConditionalBounds_m != NULL) delete graphWithConditionalBounds_m;
   setGraphAndPbPointersToNull();
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::reinitializeParametersToDefault(){
   nSolutionsSearched = 1;
   stopBound_m = 0;
   isVertexResourceActivated = true;
   algorithm_m = LabelAlgorithm::LabelSetting_withLabelMapMaxSize;
   labelMapMaxSize = 100000;
   bounds_m = Bounds::Single_Bound;
   recomputeStateGraphIfResourceModification = false;
   clusterParameters_m.clusertingSimulatedAnnealingMaxTime_s=5e-3;
   clusterParameters_m.clusteringSimulatedAnnealingMinImprovePerPeriod_percent=0.1;
   clusterParameters_m.clusteringSimulatedAnnealingPeriod_s=5e-4;
   clusterParameters_m.nbClustersPerVertex=50;
   conditionalParameters_m.delta = 1;
   conditionalParameters_m.ubCost = 0;
   isMemoryBounded = false;
   isMemoryBoundedExact = true;
   memoryBoundedMaxSize = 10000;


//   nClustersStatesPerVertex_m = 10;
}


template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::printGraphUsedByAlgorithmsToFile(string const & filename){
   updateContextGraphIfNeeded();
   contextGraph_m->getGraph()->printToFile(filename);
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::setApproximatePricing(bool b){
   if (approximatePricing_m != b){
      approximatePricing_m = b;
      Resource::setApproximatePricing(b);
      ++modificationIndices_m.resources;
   }
}

template <typename Resource, typename CostConstaintFunctions>
Resource MRCSPproblem<Resource, CostConstaintFunctions>::getResourceOfArcUsedByAlgorithm(const string & arc){
   updateContextGraphIfNeeded();
   auto logger = latticeRCSP::Console::getPtrLogger();
   auto arcIt = arcs_m.find(arc);
   if (arcIt == arcs_m.end()){
      logger->info("Asked for resource of non existing arc {}, returned supremum", arc);
      Resource res = Resource();
      res.setToSupremum();
      return res;
   }

   auto ptrArcContext = arcIt->second.getArcContextM();
   if (ptrArcContext) return ptrArcContext->getResource();
   else {
      logger->info("Asked for resource of unreachable arc {}, returned supremum", arc);
      Resource res = Resource();
      res.setToSupremum();
      return res;
   }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::setGraphAndPbPointersToNull(){
   pb_m = NULL;
   contextGraph_m = NULL;
   graphWithBounds_m = NULL;
   graphWithStateGraphBounds_m = NULL;
   graphWithConditionalBounds_m = NULL;
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::addVertex(MRCSPvertexInfo<Resource> info){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   if (vertices_m.find(info.id) == vertices_m.end()) {
      vertices_m.insert(pair<string, VertexPb>(info.id, VertexPb(info)));
   } else {
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("There is already a vertex with ID {} in MRCSPproblem<Resource, CostConstaintFunctions>::addVertex", info.id);
      throw;
   }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::deleteVertex(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   typename map<string, VertexPb >::iterator it = vertices_m.find(id);
   if (it != vertices_m.end()){
      set<string> arcs = it->second.getArcsContainingVertex();
      for (typename set<string>::iterator arcIt = arcs.begin(); arcIt!= arcs.end(); ++arcIt){
         deleteArc(*arcIt);
      }
      vertices_m.erase(it);
   } else {
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("There is no vertex with ID {} in MRCSPproblem<Resource, CostConstaintFunctions>::deleteVertex", id);
      throw;
   }
}


template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateVertexResource(string id, Resource res){
   ++modificationIndices_m.resources;
   typename map<string, VertexPb >::iterator it = vertices_m.find(id);
   if (it != vertices_m.end()){
      it->second.setResource(res);
   } else {
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("There is no vertex with ID {} in MRCSPproblem<Resource, CostConstaintFunctions>::updateVertexResource", id);
      throw;
   }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::addArc(MRCSParcInfo<Resource> info){
   ++modificationIndices_m.structure;
   if (arcs_m.find(info.id) != arcs_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("Arcs with if {} already in arcs_m in MRCSPproblem<Resource, CostConstaintFunctions>::addArc", info.id);
      throw;
   }
   auto tailIt = vertices_m.find(info.tail) ;
   auto headIt = vertices_m.find(info.head) ;
   if (headIt == vertices_m.end() || tailIt == vertices_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("Arc {}  head or tail is not in vertices_m in MRCSPproblem<Resource, CostConstaintFunctions>::addArc", info.id);
      throw;
   }
   if (info.id.find(arcSeparator) != string::npos){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("Arc id (name) {} should no contain arcSeparator: {} ", info.id, arcSeparator);
      throw;
   }


   headIt->second.addContainingArc(info.id);
   tailIt->second.addContainingArc(info.id);
   arcs_m.insert(pair<string, ArcPb>(info.id, ArcPb(info)));
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::deleteArc(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   auto it = arcs_m.find(id);
   if (it == arcs_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("delete arc {}  that is not in arcs_m in MRCSPproblem::deleteArc", id);
      throw;
   }

   auto tailIt = vertices_m.find(it->second.getTailM());
   auto headIt = vertices_m.find(it->second.getHeadM());

   headIt->second.deleteContainingArc(id);
   tailIt->second.deleteContainingArc(id);

   arcs_m.erase(id);
}


template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateArcResource(string id, Resource res){
   ++modificationIndices_m.resources;
   auto it = arcs_m.find(id);
   if (it == arcs_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("can't update arc {} bevause no such arc exisits in MRCSPproblem::updateResource", id);
      throw;
   }
   it->second.setResourceM(res);
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::addOrigin(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   if (vertices_m.find(id) == vertices_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("vertex {}  not found in MRCSPproblem::addOrigin", id);
   }
   originVertices.insert(id);
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::deleteOrigin(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   if (originVertices.find(id) == originVertices.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("delete a non-existing origin {}", id);
      throw;
   }
   originVertices.erase(id);
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::addDestination(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   if (vertices_m.find(id) == vertices_m.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("vertex {}  not found in MRCSPproblem::addDestiniation", id);
   }   destinationVertices.insert(pair<string, ArcContext<Resource> * >(id, NULL));
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::deleteDestination(string id){
   ++modificationIndices_m.structure;
//   hasStructureBeenUpdated = true;
   if (destinationVertices.find(id) == destinationVertices.end()){
      auto logger = latticeRCSP::Console::getPtrLogger();
      logger->error("delete a non-existing destination {} ", id);
      throw;
   }
   destinationVertices.erase(id);
}

template <typename Resource, typename CostConstaintFunctions>
list<string> MRCSPproblem<Resource, CostConstaintFunctions>::parseSolutionToArcList(string const & pathID){
   list<string> arcIDlist;

  string pathIDcopy = pathID;
   while (pathIDcopy.find(arcSeparator) != string::npos){
      size_t pos = pathIDcopy.find(arcSeparator, 1);
      arcIDlist.push_back(pathIDcopy.substr(arcSeparator.size(),pos - arcSeparator.size()));
      pathIDcopy.erase(0,pos);
  }
   arcIDlist.pop_front();
   arcIDlist.pop_back();
  return arcIDlist;
}

template <typename Resource, typename CostConstaintFunctions>
list<string> MRCSPproblem<Resource, CostConstaintFunctions>::parseSolutionToEmptyPathSingleVertex(const string & pathID){
   size_t pos = pathID.find(dummyOriginArcID);
   string vert = pathID.substr(0, pos);
   list<string> solution;
   solution.push_back(vert);
   return solution;
}

template <typename Resource, typename CostConstaintFunctions>
list<string> MRCSPproblem<Resource, CostConstaintFunctions>::convertArcListToVertexList(list<string> const & arcList){
   list<string> vertList;
   for (auto arcIt = arcList.begin(); arcIt != arcList.end(); ++arcIt){
      vertList.push_back(arcs_m.find(*arcIt)->second.getTailM());
   }
   auto arcIt = arcList.end();
   --arcIt;
   vertList.push_back(arcs_m.find(*arcIt)->second.getHeadM());
   return vertList;
}



template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::buildContextGraph(){
   auto logger = Console::getPtrLogger();
   logger->info("start building context graph");
   auto t0 = clock();

   if (contextGraph_m != 0){
      delete contextGraph_m; // This deletes then context arcs
      contextGraph_m = NULL;
      reinitializeArcContextPointers();
   }

   ContextBuilder<Resource> builder = ContextBuilder<Resource>();
   builder.setDummyOriginArcID(dummyOriginArcID);
   builder.setDummyDestinationArcID(dummyDestinationArcID);
   builder.setIsSinkVertexResourceActivated(isVertexResourceActivated);
   builder.setIsDummyOriginAndDestinationAdded(true);
   builder.setIsOneDummyArcCopyAddedForEachOriginVertex(true);

   NewArcContextInformation<Resource> info;
   for (auto arcIt = arcs_m.begin(); arcIt!= arcs_m.end(); ++arcIt){
      info.originVertexName = arcIt->second.getTailM();
      info.destinationVertexName = arcIt->second.getHeadM();
      info.arcContextName = arcIt->second.getIdM();
      info.arcContextID = arcIt->second.getIdM() + arcSeparator;
      info.arcContextResource = computeArcResource(arcIt->first);
      arcIt->second.setArcContextM(builder.addContextArc(info));
   }

   logger->info("arcs added after {}", (std::clock() - t0)/(double) CLOCKS_PER_SEC);

   for (auto vertIt = originVertices.begin(); vertIt != originVertices.end(); ++vertIt){
      builder.addSourceVertex(*vertIt);
   }

   for (auto vertIt = destinationVertices.begin(); vertIt != destinationVertices.end(); ++vertIt){
      builder.addSinkVertex(vertIt->first);
      if (isVertexResourceActivated) {
         NewArcContextOfSinkVertexInformation<Resource> info;
         info.sinkVertexName = vertIt->first;
         auto verticesMit = vertices_m.find(vertIt->first);
         assert((verticesMit != vertices_m.end()));
         info.arcContextResource = verticesMit->second.getResource();
         info.arcContextID = "";
         vertIt->second = builder.addContextArcOfSinkVertex(info);
      }
   }

   logger->info("everything added after {}",(std::clock() - t0)/(double) CLOCKS_PER_SEC);
   contextGraph_m = builder.getContextGraphAndRenameUnreachableContextArcs();
   logger->info("context built after {}", (std::clock() - t0)/(double) CLOCKS_PER_SEC);


   modificationsContextGraph = modificationIndices_m;
//   hasStructureBeenUpdated = false;
//   haveResourcesBeenUpdated = false;
}

template <typename Resource, typename CostConstaintFunctions>
Resource MRCSPproblem<Resource, CostConstaintFunctions>::computeArcResource(const string & arc){
   auto arcIt = arcs_m.find(arc);
   Resource res;
   if (isVertexResourceActivated){
      auto vertIt = vertices_m.find(arcIt->second.getTailM());
      if (vertIt == vertices_m.end()){
         auto logger = latticeRCSP::Console::getPtrLogger();
         logger->error("Tail of {} is {} and not in vertices_m in MRCSPproblem::::buildContextGraph()", arcIt->first, arcIt->second.getTailM());
         throw;
      }
      res = vertIt->second.getResource();
      res.expand(arcIt->second.getResourceM());
   } else {
      res = arcIt->second.getResourceM();
   }
   return res;
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateContextArcResources(){
   assert(contextGraph_m != 0);
   assert(modificationIndices_m.structure == modificationsContextGraph.structure);

   for (auto arcIt = arcs_m.begin(); arcIt != arcs_m.end(); ++arcIt){
	arcIt->second.getArcContextM()->setResource(computeArcResource(arcIt->first));
   }


    if (isVertexResourceActivated){
       for (auto destIt = destinationVertices.begin(); destIt != destinationVertices.end(); ++destIt){
          auto vertIt = vertices_m.find(destIt->first);
          assert((vertIt != vertices_m.end()));
          destIt->second->setResource(vertIt->second.getResource());
       }
    }
#ifdef PROFILE_MRCSP
    cout << (clock() - t0)/(double) CLOCKS_PER_SEC << " updatingArcContextResources" << endl;
#endif


   modificationsContextGraph.resources = modificationIndices_m.resources;
//   haveResourcesBeenUpdated = false;
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::reinitializeArcContextPointers(){
   for (auto arcIt = arcs_m.begin(); arcIt!= arcs_m.end(); ++arcIt){
      arcIt->second.setArcContextM(NULL);
   }

   if (isVertexResourceActivated) {
      for (auto destIt = destinationVertices.begin(); destIt != destinationVertices.end(); ++destIt){
         auto logger = Console::getPtrLogger();
        logger->trace("ptr reinitialized {}", destIt->first);
      destIt->second = NULL;
   }

   }
}

//template <typename Resource, typename CostConstaintFunctions>
//ContextGraph<Resource> * MRCSPproblem<Resource, CostConstaintFunctions>::getContextGraph_rebuildItIfNeeded(){
//   if (hasStructureBeenUpdated){
//      buildContextGraph();
//   }
//   if (haveResourcesBeenUpdated){
//      updateContextArcResources();
//   }
//   return contextGraph_m;
//}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::setNSolutionsSearched(int n){
   nSolutionsSearched = n;
   if (pb_m != 0){
      pb_m->setNumberOfSolutionsSearched(nSolutionsSearched);
   }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateContextGraphIfNeeded(){
   if (modificationsContextGraph.structure < modificationIndices_m.structure){
      buildContextGraph();
   } else if (modificationsContextGraph.resources < modificationIndices_m.resources) {
      updateContextArcResources();
   }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateProblemIfNeeded(){
    if (!(modificationIndices_m == modificationsPb)){
       if (pb_m != 0) {
         delete pb_m;
         pb_m = nullptr;
       }

       updateContextGraphIfNeeded();
       pb_m = new Pb(contextGraph_m);
       pb_m->setNumberOfSolutionsSearched(nSolutionsSearched);
       modificationsPb = modificationIndices_m;
    }
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateGraphWithBoundsIfNeeded(){
	clock_t t0 = clock();
	updateProblemIfNeeded();
	clock_t t1 = clock();

   if (modificationsGraphWithBounds.structure < modificationIndices_m.structure ){
      if (graphWithBounds_m != 0) delete graphWithBounds_m;
      graphWithBounds_m = new GraphWithBounds<Resource, Pb>(contextGraph_m->getGraph(), pb_m);
   }
   if (modificationsGraphWithBounds.resources < modificationIndices_m.resources \
      || modificationsGraphWithBounds.structure < modificationIndices_m.structure){
	  graphWithBounds_m->setProblem(pb_m);
      graphWithBounds_m->setReverseBoundAndCandidatePath();
   }
   modificationsGraphWithBounds = modificationIndices_m;
#ifdef PROFILE_MRCSP
   clock_t t2 = clock();
   cout << (t1 - t0)/(double) CLOCKS_PER_SEC << " MRCSP resource updating, " << endl;
   cout << (t2-t1)/(double) CLOCKS_PER_SEC << "MRCSP bounds updating." << endl;
#endif

}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::computeGraphWithStateGraphBounds(){
	graphWithStateGraphBounds_m->setProblem(pb_m);
	graphWithStateGraphBounds_m->setClusertingSimulatedAnnealingMaxTime_s(clusterParameters_m.clusertingSimulatedAnnealingMaxTime_s);
	graphWithStateGraphBounds_m->setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(clusterParameters_m.clusteringSimulatedAnnealingMinImprovePerPeriod_percent);
	graphWithStateGraphBounds_m->setClusteringSimulatedAnnealingPeriod_s(clusterParameters_m.clusteringSimulatedAnnealingPeriod_s);
	graphWithStateGraphBounds_m->template setReverseBoundAndCandidatePath<ConstraintHeuristicPb>(clusterParameters_m.nbClustersPerVertex);
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateGraphWithStateGraphBoundsIfNeeded(){
	updateProblemIfNeeded();
	if (modificationsGraphWithStateGraphBounds.structure < modificationIndices_m.structure ){
		if (graphWithStateGraphBounds_m != 0) delete graphWithStateGraphBounds_m;
		auto copy = *contextGraph_m->getGraph();
		graphWithStateGraphBounds_m = new GraphWithStateGraphBounds<Resource, Pb>(copy, pb_m);
		graphWithStateGraphBounds_m->setIsStateGraphReusable(!recomputeStateGraphIfResourceModification);
	}

//	recomputeStateGraphIfResourceModification = true;
	if (recomputeStateGraphIfResourceModification){
		if ( modificationsGraphWithStateGraphBounds.resources < modificationIndices_m.resources \
			  || modificationsGraphWithStateGraphBounds.structure < modificationIndices_m.structure){
			computeGraphWithStateGraphBounds();
		}
	} else {
		if (modificationsGraphWithStateGraphBounds.structure < modificationIndices_m.structure || \
				(modificationsGraphWithStateGraphBounds.resources < modificationIndices_m.resources && !graphWithStateGraphBounds_m->getIsReverseBoundAndCandidatePathSet()) ){
			computeGraphWithStateGraphBounds();
		}
		else if (modificationsGraphWithStateGraphBounds.resources < modificationIndices_m.resources){
			graphWithStateGraphBounds_m->setProblem(pb_m);
			graphWithStateGraphBounds_m->reverseGraph();
			graphWithStateGraphBounds_m->updateDirectionBoundAndPathOnSameStateGraph();
			graphWithStateGraphBounds_m->reverseGraph();
		}
	}
	modificationsGraphWithStateGraphBounds = modificationIndices_m;
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::updateGraphWithConditionalBoundsIfNeeded(){
	updateProblemIfNeeded();
	if (modificationsGraphWithConditionalBounds.structure < modificationIndices_m.structure){
			}
	if (modificationsGraphWithConditionalBounds.resources < modificationIndices_m.resources \
	      || modificationsGraphWithConditionalBounds.structure < modificationIndices_m.structure){
		if (graphWithConditionalBounds_m != 0) delete graphWithConditionalBounds_m;
		auto copy = *contextGraph_m->getGraph();
		graphWithConditionalBounds_m = new GraphWithConditionalBounds<Resource, Pb, CostPb>(copy, pb_m);
		graphWithConditionalBounds_m->setProblem(pb_m);
		graphWithConditionalBounds_m->reverseGraph();
		graphWithConditionalBounds_m->setOriginDestinationMaxConditionalValue(conditionalParameters_m.ubCost);
		graphWithConditionalBounds_m->setDirectionConditionalGraphsBoundsAndPath(conditionalParameters_m.delta);
		graphWithConditionalBounds_m->addDirectionConditionalPathOfDestinationAsProblemSolution();
		graphWithConditionalBounds_m->reverseGraph();
//		graphWithConditionalBounds_m->setReverseBoundAndCandidatePath(); (already set in setDirectionConditionalGraphsBoundsAndPath)
   }
   modificationsGraphWithConditionalBounds = modificationIndices_m;
}

template <typename Resource, typename CostConstaintFunctions>
void MRCSPproblem<Resource, CostConstaintFunctions>::solve(){
	if (bounds_m == Bounds::Single_Bound){
		updateGraphWithBoundsIfNeeded();
		if (algorithm_m == LabelAlgorithm::LabelSetting_withLabelMapMaxSize){
			LabelSettingAlgorithm_withLabelMapMaxSize<Resource, GraphWithBounds<Resource, Pb> > label(graphWithBounds_m, labelMapMaxSize);
			label.setLabelMapMaxSize(labelMapMaxSize);
			label.setIsStopBoundActivated(isStopBoundActivated_m);
			label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
			label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
			labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelCorrecting_withLabelMapMaxSize){
		  LabelCorrectingAlgorithm_withLabelMapMaxSize<Resource, GraphWithBounds<Resource, Pb> > label(graphWithBounds_m, labelMapMaxSize);
		  label.setLabelMapMaxSize(labelMapMaxSize);
        label.setIsStopBoundActivated(isStopBoundActivated_m);
        label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
        label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
		labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelDominance_withLabelMapMaxSize){
		  LabelDominanceAlgorithm_withLabelMapMaxSize<Resource, GraphWithBounds<Resource, Pb> > label(graphWithBounds_m, labelMapMaxSize);
		  label.setLabelMapMaxSize(labelMapMaxSize);
        label.setIsStopBoundActivated(isStopBoundActivated_m);
        label.setStopBound(stopBound_m);
        if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
        label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
		labelResult = label.getResults();
		}
	}
	else if (bounds_m == Bounds::Clustered_Bounds){
		updateGraphWithStateGraphBoundsIfNeeded();
		if (algorithm_m == LabelAlgorithm::LabelSetting_withLabelMapMaxSize){
			LabelSettingAlgorithm_withLabelMapMaxSize<Resource, GraphWithStateGraphBounds<Resource, Pb> > label(graphWithStateGraphBounds_m, labelMapMaxSize);
			label.setLabelMapMaxSize(labelMapMaxSize);
			label.setIsStopBoundActivated(isStopBoundActivated_m);
			label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
			label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
			labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelCorrecting_withLabelMapMaxSize){
			LabelCorrectingAlgorithm_withLabelMapMaxSize<Resource, GraphWithStateGraphBounds<Resource, Pb> > label(graphWithStateGraphBounds_m, labelMapMaxSize);
			label.setLabelMapMaxSize(labelMapMaxSize);
			label.setIsStopBoundActivated(isStopBoundActivated_m);
			label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
			label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
			labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelDominance_withLabelMapMaxSize){
			auto logger = latticeRCSP::Console::getPtrLogger();
			logger->error("Label Dominance algorithm cannot be used with a state graph");
		}
	}
	else if (bounds_m == Bounds::Conditional_Bounds){
		BoundAndPathMap<Resource, Pb, CostPb>::setCostConditionalFunctorRelation(CostConditionalFunctorRelation::COST_IS_CONDITIONAL_FUNCTOR);
		updateGraphWithConditionalBoundsIfNeeded();
		if (algorithm_m == LabelAlgorithm::LabelSetting_withLabelMapMaxSize){
			LabelSettingAlgorithm_withLabelMapMaxSize<Resource, GraphWithConditionalBounds<Resource, Pb, CostPb> > label(graphWithConditionalBounds_m, labelMapMaxSize);
			label.setLabelMapMaxSize(labelMapMaxSize);
			label.setIsStopBoundActivated(isStopBoundActivated_m);
			label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
			label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
			labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelCorrecting_withLabelMapMaxSize){
			LabelCorrectingAlgorithm_withLabelMapMaxSize<Resource, GraphWithConditionalBounds<Resource, Pb, CostPb> > label(graphWithConditionalBounds_m, labelMapMaxSize);
			label.setLabelMapMaxSize(labelMapMaxSize);
			label.setIsStopBoundActivated(isStopBoundActivated_m);
			label.setStopBound(stopBound_m);
         if (isMemoryBounded){
            label.setIsMemoryBounded(isMemoryBounded);
            label.setMemoryBoundedMaxSize(memoryBoundedMaxSize);
            isMemoryBoundedExact = true;
         }
			label.solve();
         if (isMemoryBounded) isMemoryBoundedExact = label.getIsMemoryBoundedExact();
			labelResult = label.getResults();
		} else if (algorithm_m == LabelAlgorithm::LabelDominance_withLabelMapMaxSize){
			auto logger = latticeRCSP::Console::getPtrLogger();
			logger->error("Label Dominance algorithm cannot be used with a conditional graph");
		}
	}
	else {
		auto logger = latticeRCSP::Console::getPtrLogger();
		logger->error("Bounds selected are still not implemented");
	}
}

template <typename Resource, typename CostConstaintFunctions>
multimap<double, pair<list<string>, Resource > > MRCSPproblem<Resource, CostConstaintFunctions>::getSolutionsArcList(){
   map<string, Path<Resource> * > const * ptrSolutionMap = pb_m->getSolutionSet()->getSolution();
   multimap<double, pair< list<string>, Resource > > solution;
//   map<string, Path<Resource> * >::const_iterator it;
   for (auto it = ptrSolutionMap->begin(); it!= ptrSolutionMap->end(); ++it){
      Resource const & res = it->second->getResource();
      double key = CostConstaintFunctions::getCost(res);
      list<string> listSol = parseSolutionToArcList(it->first);
      pair <double, pair< list<string>, Resource > > resu(key, pair< list<string>, Resource >(listSol, res));
      solution.insert(pair <double, pair< list<string>, Resource > >(key, pair< list<string>, Resource >(listSol, res)));
   }
   return solution;
}

template <typename Resource, typename CostConstaintFunctions>
multimap<double, pair<list<string>, Resource > > MRCSPproblem<Resource, CostConstaintFunctions>::getSolutionsVertexList(){
      map<string, Path<Resource> * > const * ptrSolutionMap = pb_m->getSolutionSet()->getSolution();
      multimap<double, pair< list<string>, Resource > > solution;
   //   map<string, Path<Resource> * >::const_iterator it;
      for (auto it = ptrSolutionMap->begin(); it!= ptrSolutionMap->end(); ++it){
         Resource const & res = it->second->getResource();
         double key = CostConstaintFunctions::getCost(res);
         list<string> listArc = parseSolutionToArcList(it->first);
         list<string> listVert;
         if (listArc.empty()){
           listVert = parseSolutionToEmptyPathSingleVertex(it->first);
         }
         else {
            listVert = convertArcListToVertexList(listArc);
         }
//       pair <double, pair< list<string>, Resource > > resu(key, pair< list<string>, Resource >(listSol, res));
         solution.insert(pair <double, pair< list<string>, Resource > >(key, pair< list<string>, Resource >(listVert, res)));
      }
      return solution;
//   multimap<double, pair<list<string>, Resource > > arcSolution = getSolutionsArcList();
//   multimap<double, pair<list<string>, Resource > > vertexSolution;
//   for (auto it = arcSolution.begin(); it != arcSolution.end(); ++it){
//      vertexSolution.insert(pair<double, pair<list<string>, Resource > >(\
//         it->first, pair<list<string>, Resource >(\
//            convertArcListToVertexList(it->second.first),\
//            it->second.second)));
//   }
//   return vertexSolution;
}

template <typename Resource, typename CostConstaintFunctions>
Resource MRCSPproblem<Resource, CostConstaintFunctions>::rebuildPathResource(list<string> const & arcsList)
{
	return contextGraph_m->rebuildPathForwardFromString(arcsList).getResource();
}

template <typename Resource, typename CostConstaintFunctions>
Resource MRCSPproblem<Resource, CostConstaintFunctions>::rebuildPathResourceFromEnd(list<string> const & arcsList)
{
	return contextGraph_m->rebuildPathBackwardFromString(arcsList).getResource();
}


} /* namespace mrcsp */
