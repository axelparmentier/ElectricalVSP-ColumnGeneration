//
//  GraphWithStateGraphBounds.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "GraphWithStateGraphBounds.h"
#include "../Clustering/Clusterization.h"
#include "../Clustering/Cluster.h"
#include "../Clustering/ClusterFunctors.h"
#include "../Resources/Functors/CostFunction.h"


namespace latticeRCSP {
    //-----------------------------
    // Constructor and getters
    //-----------------------------
    template <typename Resource, typename Problem>
    GraphWithStateGraphBounds<Resource,Problem>::GraphWithStateGraphBounds(Graph<Resource> & graph0, Problem * problem0) : Graph<Resource>(graph0){
        pProblem_m = problem0;
        isBoundAndPathFromOriginToVertexSet_m = false;
        isBoundAndPathFromVertexToDestinationSet_m = false;
        
        stateGraphFromOriginToVertex = (StateGraph<Resource, Problem> *)(0);
        
        stateGraphFromVertexToDestination = (StateGraph<Resource, Problem> *)(0);
        
        period_s = 0.1;
        minImproveOnPeriod_percent = 0.2;
        tMax_s = 2;

    }

    
    template <typename Resource, typename Problem>
    GraphWithStateGraphBounds<Resource,Problem>::~GraphWithStateGraphBounds(){
        if (stateGraphFromOriginToVertex != 0) delete stateGraphFromOriginToVertex;
        if (stateGraphFromVertexToDestination != 0) delete stateGraphFromVertexToDestination;
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithStateGraphBounds<Resource, Problem>::getIsBoundSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isBoundAndPathFromOriginToVertexSet_m;
        } else {
            return isBoundAndPathFromVertexToDestinationSet_m;
        }
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithStateGraphBounds<Resource, Problem>::getIsReverseBoundAndCandidatePathSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isBoundAndPathFromOriginToVertexSet_m;
        } else {
            return isBoundAndPathFromVertexToDestinationSet_m;
        }
    }
    
    template <typename Resource, typename Problem>
    template<typename VerticesOrderMorphismToRFunctor>
    void GraphWithStateGraphBounds<Resource, Problem>::setReverseBoundAndCandidatePath(int nStatePerVertices){

    	typedef Clusterization<Resource, Cluster<Resource, SumOfDistancesToMeetFunctor<Resource > > > ClusterSolver;

    	Graph<Resource>::reverseGraph();
    	if (Graph<Resource>::getIsAcyclic()){

    		setDirectionStateGraphBoundAndPath_acyclic<ClusterSolver>(nStatePerVertices);
    	} else {
    		int firstDirection = 1 + (2 * nStatePerVertices) /3 ;
    		setDirectionStateGraphBoundAndPath< VerticesOrderMorphismToRFunctor , ClusterSolver>(firstDirection, nStatePerVertices - firstDirection);
    	}

    	Graph<Resource>::reverseGraph();
    }
    
    template <typename Resource, typename Problem>
    StateGraphBuildingResults GraphWithStateGraphBounds<Resource, Problem>::getBuildingResults() const{
       if (Graph<Resource>::getIsReverse()){
          return stateGraphFromVertexToDestination->getBuildingResults();
       } else {
          return stateGraphFromOriginToVertex->getBuildingResults();
       }
    }

    template <typename Resource, typename Problem>
     MeetAlgorithmResults GraphWithStateGraphBounds<Resource, Problem>::getReverseMeetAlgorithmResults() const{
        if (Graph<Resource>::getIsReverse()){
           return stateGraphFromOriginToVertex->getMeetResults();
        } else {
           return stateGraphFromVertexToDestination ->getMeetResults();
        }
     }

   template <typename Resource, typename Problem>
   DijkstraLikeHeuristicResults GraphWithStateGraphBounds<Resource, Problem>::getReverseDijkstraHeuristicResults() const{
      if (Graph<Resource>::getIsReverse()){
         return stateGraphFromOriginToVertex->getDijkstraHeuristicResults();
      } else {
         return stateGraphFromVertexToDestination ->getDijkstraHeuristicResults();
      }
   }

    template <typename Resource, typename Problem>
    Problem * GraphWithStateGraphBounds<Resource,Problem>::getProblem() const{
        return pProblem_m;
    }
    
    template <typename Resource, typename Problem>
    void GraphWithStateGraphBounds<Resource, Problem>::setClusteringSimulatedAnnealingPeriod_s(double t){
        period_s = t;
    }
    
    
    template <typename Resource, typename Problem>
    void GraphWithStateGraphBounds<Resource, Problem>::setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(double val){
        minImproveOnPeriod_percent = val;
    }
    
    template <typename Resource, typename Problem>
    void GraphWithStateGraphBounds<Resource, Problem>::setClusertingSimulatedAnnealingMaxTime_s(double t){
        tMax_s = t;
    }
    
    //-----------------------------
    // Bounded Graph Domination Functions
    //-----------------------------
    
    template <typename Resource, typename Problem>
    bool GraphWithStateGraphBounds<Resource, Problem>::isDominated(int vertex, const Resource & r) const{
        Resource completePath = r;
        if (Graph<Resource>::getIsReverse()) {
            return boundSetOnSolutionFromOriginToVertex[vertex].testCompleteDominated(r,Graph<Resource>::getIsReverse());
        } else{
            return boundSetOnSolutionFromVertexToDestination[vertex].testCompleteDominated(r,Graph<Resource>::getIsReverse());
        }
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithStateGraphBounds<Resource, Problem>::isDominated(const Resource &r) const{
        return pProblem_m->isResourceInfeasibleOrDominatedByASolution(r);
        
    }
    
    
    template <typename Resource, typename Problem>
    bool GraphWithStateGraphBounds<Resource, Problem>::testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> & pathTested , double & lowerBoundOnCostFromPath) const{
        
        int vertex = pathTested.getDestination();
        if (Graph<Resource>::getIsReverse()) {
            //            cout << "this test" << endl;
            if (!isBoundAndPathFromOriginToVertexSet_m) throw "not set";
            return boundSetOnSolutionFromOriginToVertex[vertex].testCompleteDominatedAndUpdateProblemSolution(pathTested, lowerBoundOnCostFromPath,Graph<Resource>::getIsReverse());
        } else{
            if (!isBoundAndPathFromVertexToDestinationSet_m) throw "not set";
            return boundSetOnSolutionFromVertexToDestination[vertex].testCompleteDominatedAndUpdateProblemSolution(pathTested,  lowerBoundOnCostFromPath,Graph<Resource>::getIsReverse());
        }
    }
    
    //-----------------------------
    // Building State Graphs
    //-----------------------------
    
    template <typename Resource, typename Problem>
    template<typename ClusterizationSolver>
    void GraphWithStateGraphBounds<Resource, Problem>::setDirectionStateGraphBoundAndPath_acyclic(int nStatesPerVertex){
        if (Graph<Resource>::getIsReverse()) {
        	if (stateGraphFromVertexToDestination != 0){
				delete stateGraphFromVertexToDestination;
			}
			stateGraphFromVertexToDestination = new StateGraph<Resource, Problem>(Graph<Resource>::getIsReverse(), this);
			stateGraphFromVertexToDestination->setIsStateGraphReusable(isStateGraphReusable_m);
			initializeStateGraphParameter(stateGraphFromVertexToDestination);
            stateGraphFromVertexToDestination->template buildStateGraph_acyclic<ClusterizationSolver>(nStatesPerVertex);
            boundSetOnSolutionFromVertexToDestination = stateGraphFromVertexToDestination->getDirectionBoundAndPathList();
            
            isBoundAndPathFromVertexToDestinationSet_m = true;
        } else {
        	if (stateGraphFromOriginToVertex != 0){
				delete stateGraphFromOriginToVertex;
			}
			stateGraphFromOriginToVertex = new StateGraph<Resource, Problem>(Graph<Resource>::getIsReverse(), this);
			stateGraphFromOriginToVertex->setIsStateGraphReusable(isStateGraphReusable_m);
			initializeStateGraphParameter(stateGraphFromOriginToVertex);
			stateGraphFromOriginToVertex->template buildStateGraph_acyclic<ClusterizationSolver>(nStatesPerVertex);
            boundSetOnSolutionFromOriginToVertex = stateGraphFromOriginToVertex->getDirectionBoundAndPathList();
            isBoundAndPathFromOriginToVertexSet_m = true;
        }
    }
    
    template <typename Resource, typename Problem>
    template<typename VerticesOrderMorphismToRFunctor, typename ClusterizationSolver>
    void GraphWithStateGraphBounds<Resource, Problem>::setDirectionStateGraphBoundAndPath(int nStatesPerVertexFirstDirection, int nStatesPerVertexSecondDirection){
        if (Graph<Resource>::getIsReverse()) {
            if (stateGraphFromVertexToDestination != 0){
                delete stateGraphFromVertexToDestination;
            }
            stateGraphFromVertexToDestination = new StateGraph<Resource, Problem>(Graph<Resource>::getIsReverse(), this);
			stateGraphFromVertexToDestination->setIsStateGraphReusable(isStateGraphReusable_m);
            initializeStateGraphParameter(stateGraphFromVertexToDestination);
            stateGraphFromVertexToDestination->template buildStateGraph_withCycle<VerticesOrderMorphismToRFunctor,ClusterizationSolver>(nStatesPerVertexFirstDirection,nStatesPerVertexSecondDirection);
            boundSetOnSolutionFromVertexToDestination = stateGraphFromVertexToDestination->getDirectionBoundAndPathList();
            
            isBoundAndPathFromVertexToDestinationSet_m = true;
        } else {
            if (stateGraphFromOriginToVertex != 0){
                delete stateGraphFromOriginToVertex;
            }
            stateGraphFromOriginToVertex = new StateGraph<Resource, Problem>(Graph<Resource>::getIsReverse(), this);
			stateGraphFromOriginToVertex->setIsStateGraphReusable(isStateGraphReusable_m);
            initializeStateGraphParameter(stateGraphFromOriginToVertex);
            stateGraphFromOriginToVertex->template buildStateGraph_withCycle<VerticesOrderMorphismToRFunctor,ClusterizationSolver>(nStatesPerVertexFirstDirection,nStatesPerVertexSecondDirection);
            boundSetOnSolutionFromOriginToVertex = stateGraphFromOriginToVertex->getDirectionBoundAndPathList();
            isBoundAndPathFromOriginToVertexSet_m = true;
        }
    }
    
    template <typename Resource, typename Problem>
    void GraphWithStateGraphBounds<Resource, Problem>::initializeStateGraphParameter(StateGraph<Resource, Problem> * pGraph){
        pGraph->setClusteringSimulatedAnnealingPeriod_s(period_s);
        pGraph->setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(minImproveOnPeriod_percent);
        pGraph->setClusertingSimulatedAnnealingMaxTime_s(tMax_s);
    }

    template <typename Resource, typename Problem>
	void GraphWithStateGraphBounds<Resource, Problem>::updateDirectionBoundAndPathOnSameStateGraph(){
    	if (!isStateGraphReusable_m) throw;
    	if (Graph<Resource>::getIsReverse()) {
    		if (stateGraphFromVertexToDestination == 0) {
    			cerr << "non initialized state graph" << endl;
    			throw;
    		}
    		// Commented because recomputed on next line
//    		stateGraphFromVertexToDestination->GraphWithBounds<Resource, Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
//    		stateGraphFromVertexToDestination->GraphWithBounds<Resource, Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();

    		stateGraphFromVertexToDestination->setProblem(pProblem_m);
    		boundSetOnSolutionFromVertexToDestination = stateGraphFromVertexToDestination->getDirectionBoundAndPathList();
			isBoundAndPathFromVertexToDestinationSet_m = true;
    	} else {
    		if (stateGraphFromOriginToVertex == 0) {
				cerr << "non initialized state graph" << endl;
				throw;
			}
//    		stateGraphFromOriginToVertex->GraphWithBounds<Resource, Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
//    		stateGraphFromOriginToVertex->GraphWithBounds<Resource, Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
    		stateGraphFromOriginToVertex->setProblem(pProblem_m);
    		boundSetOnSolutionFromOriginToVertex = stateGraphFromOriginToVertex->getDirectionBoundAndPathList();
    		isBoundAndPathFromOriginToVertexSet_m = true;
    	}
    }

    
}
