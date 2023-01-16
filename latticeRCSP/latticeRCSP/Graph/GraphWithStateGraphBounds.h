//
//  GraphWithStateGraphBounds.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__GraphWithStateGraphBounds__
#define __latticeRCSP_v2__GraphWithStateGraphBounds__

#include <stdio.h>
#include "Graph.h"
#include "StateGraph.h"
#include "BoundAndPathList.h"

namespace latticeRCSP {
    template <typename Resource, typename Problem>
    class GraphWithStateGraphBounds : public Graph<Resource> {
    protected:
        Problem * pProblem_m;
        
        bool isStateGraphReusable_m;
        bool isBoundAndPathFromOriginToVertexSet_m;
        bool isBoundAndPathFromVertexToDestinationSet_m;
        
        vector<BoundAndPathList<Resource, Problem> > boundSetOnSolutionFromOriginToVertex;
        vector<BoundAndPathList<Resource, Problem> > boundSetOnSolutionFromVertexToDestination;
        
        StateGraph<Resource, Problem> * stateGraphFromOriginToVertex;
        StateGraph<Resource, Problem> * stateGraphFromVertexToDestination;
        
        //------------------------------------------------------------------
        // Simulated annealing for clusterization parameter
        //-----------------------------------------------------------------
        double period_s;
        double minImproveOnPeriod_percent;
        double tMax_s;
        
    public:
        //-----------------------------
        // Constructor and getters
        //-----------------------------
        GraphWithStateGraphBounds<Resource, Problem>(Graph<Resource> &, Problem *);
        ~GraphWithStateGraphBounds<Resource, Problem>();
        
        //-----------------------------
        // Getters and setters
        //-----------------------------
        Problem * getProblem() const;
        bool getIsBoundSet() const;
        bool getIsReverseBoundAndCandidatePathSet() const;


        template<typename VerticesOrderMorphismToRFunctor>
        void setReverseBoundAndCandidatePath(int nStatePerVertices);

        StateGraphBuildingResults getBuildingResults() const;
        MeetAlgorithmResults getReverseMeetAlgorithmResults() const;
        DijkstraLikeHeuristicResults getReverseDijkstraHeuristicResults() const;
        
        void setClusteringSimulatedAnnealingPeriod_s(double);
        void setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(double);
        void setClusertingSimulatedAnnealingMaxTime_s(double);
        void setProblem(Problem * p){pProblem_m = p;}
        void setIsStateGraphReusable(bool isReusable){isStateGraphReusable_m = isReusable;
        	if (stateGraphFromOriginToVertex != 0 || stateGraphFromVertexToDestination != 0) throw; }
        
        
        //-----------------------------
        // Bounded Graph Domination Functions
        //-----------------------------
        bool isDominated(int vertex, const Resource &) const;
        bool isDominated(const Resource &) const;
        bool testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
        
        //-----------------------------
        // Building State Graphs
        //-----------------------------
        template<typename ClusterizationSolver>
        void setDirectionStateGraphBoundAndPath_acyclic(int nStatesPerVertex);
        template<typename VerticesOrderMorphismToRFunctor, typename ClusterizationSolver>
        void setDirectionStateGraphBoundAndPath(int nStatesPerVertexFirstDirection, int nStatesPerVertexSecondDirection);
        void initializeStateGraphParameter(StateGraph<Resource, Problem> *);
        
        //-----------------------------
		// Updating State Graphs
		//-----------------------------
		void updateDirectionBoundAndPathOnSameStateGraph();

        
    };
}
#include "GraphWithStateGraphBounds.hpp"

#endif /* defined(__latticeRCSP_v2__GraphWithStateGraphBounds__) */

