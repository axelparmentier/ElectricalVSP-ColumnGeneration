//
//  StateGraph.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__StateGraph__
#define __latticeRCSP_v2__StateGraph__

#include <stdio.h>
#include "Graph.h"
#include "BoundAndPathList.h"
#include "GraphWithBounds.h"
#include "Results/StateGraphBuildingResults.h"

template<typename Resource>
struct IncomingStateArcInformation{
    int originState;
    int networkArc;
    Resource resource_m;
};

template<typename Resource>
struct NewStateInformation{
    Resource stateResource;
    vector<IncomingStateArcInformation<Resource> > elementsInClusterInformation;
};

struct StateArcInformation{
    int origin;
    int destination;
    int networkArc;
};

namespace latticeRCSP {
    
    template <typename Resource, typename Problem>
    class GraphWithStateGraphBounds;
    
    template <typename Resource, typename Problem>
    class StateGraph : public GraphWithBounds<Resource, Problem> {
    protected:
        
        GraphWithStateGraphBounds<Resource, Problem> * conditionnedGraph_m;
        
        bool isStateGraphInitialized_m;
        bool const isStateGraphReverse_m;
        bool isStateGraphReusable_m;
        
        vector<vector<int> > stateVerticesOfVertex;
        vector<int > vertexOfStateVertex;
        vector<vector<int> > stateArcsOfArc;
        vector<int > arcOfStateArc;
        
        //------------------------------------------------------------------
        // Direction Building structures
        //-----------------------------------------------------------------
        vector<int> pseudoTopologicalOrder_m;
        int pseudoTopologicalOrder(int);
        vector<vector<IncomingStateArcInformation<Resource> > >vertexIncomingResources;
        list<Resource> stateResource;
        list<vector<Resource> > stateClusterResourcesList;
        vector<vector<Resource> > stateClusterResources;
        
        list<StateArcInformation> stateArcsList;
        vector<list<int> > stateArcsOfNetworkArcList;
        vector<list<int> > stateVerticesOfNetworkVertexList;
        
        // Not to be cleared
        set<int> directionOrigin;
        set<int> directionDestination;
        set<int> directionOriginStates;
        
        //------------------------------------------------------------------
        // Simulated annealing for clusterization parameter
        //-----------------------------------------------------------------
        double period_s;
        double minImproveOnPeriod_percent;
        double tMax_s;

        //------------------------------------------------------------------
        // Simulated annealing for clusterization parameter
        //-----------------------------------------------------------------
        StateGraphBuildingResults buildingResults_m;
        
    public:
        //------------------------------------------------------------------
        // Constructor
        //------------------------------------------------------------------
        StateGraph(bool, GraphWithStateGraphBounds<Resource, Problem> *);
        
        //------------------------------------------------------------------
        // Getters and setters
        //------------------------------------------------------------------
        vector<BoundAndPathList<Resource, Problem> > getDirectionBoundAndPathList();
        StateGraphBuildingResults getBuildingResults() const {return buildingResults_m;}
        
        void setClusteringSimulatedAnnealingPeriod_s(double);
        void setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(double);
        void setClusertingSimulatedAnnealingMaxTime_s(double);
        void setIsStateGraphReusable(bool isReusable){isStateGraphReusable_m = isReusable; if (isStateGraphInitialized_m) throw; }
        //------------------------------------------------------------------
        // Redefine the direction Lower Bound Algorithm To Use The Conditionnal Bound (in MeetAlgorithm of GraphWithBounds)
        //------------------------------------------------------------------
        
        virtual vector<Resource *> directionMeetShortestPathAlgorithm();

        //------------------------------------------------------------------
        // State Graph Building
        //------------------------------------------------------------------
        template<typename ClusterizationSolver>
        void buildStateGraph_acyclic(int nStatesPerVertex);

        template<typename VerticesOrderMorphismToRFunctor, typename ClusterizationSolver>
        void buildStateGraph_withCycle(int nStatesFirstDirection, int nStatesSecondDirection);
        
    protected:
        bool isBuildingDirectionReverse;
        void reverseBuildingDirection();
        void initializeStateGraphStructures(int);
        template<typename VerticesOrderMorphismToRFunctor>
        void buildOrderFromMorphismToR();
        
        // direction build state graph
        template<typename ClusterizationSolver>
        void directionBuildStateGraph(int nNewStatesPerVertex, bool newOriginState);
        void initializeDirectionBuildingStructures();
        void updateState(int state, NewStateInformation<Resource> const &);
        int buildStateInList(int networkVertex, NewStateInformation<Resource> const &);
        bool isResourceDominatedByOneStateOfTheVertex(int vertex, Resource const & testedResource);
        void expandState(int vertex, int state, Resource const & stateResource);
        template<typename ClusterizationSolver>
        void clusterizeIncomingResources(int nNewStatePerVertex, int vertex, vector<NewStateInformation<Resource> > & existingStatesUpdateInformation, vector<NewStateInformation<Resource> > & newStates);
        void rebuildDirectionStateVerticesAndArcsFromList();
        void cleanDirectionBuildingStructures_exceptVertexIncomingResources();
        
        void setStateGraphOriginAndDestination();
       
        //------------------------------------------------------------------
        // Conversion
        //------------------------------------------------------------------
        void convertPathFromConditionalToConditionned(Path<Resource> & path0) const;
        
    };
    
    
    template <typename Resource, typename Functor>
    class PlusEpsilonFunctor{
    public:
        static double getKey(Resource const &);
    };
}
#include "StateGraph.hpp"


#endif /* defined(__latticeRCSP_v2__StateGraph__) */
