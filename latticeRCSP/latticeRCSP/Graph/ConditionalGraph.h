//
//  ConditionalGraph.h
//  RCSP
//
//  Created by Axel Parmentier on 25/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__ConditionalGraph__
#define __RCSP__ConditionalGraph__

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <math.h>

#include "Graph.h"
#include "BoundAndPathMap.h"
#include "MeetAlgorithmFunctors/CostConstraintConditionalBoundMeetAlgorithmFunctor.h"
#include "../Tools/ProgressBar.h"

#include "Results/ConditionalGraphBuildingResults.h"
#include "GraphWithBounds.h"

#define EPS 1e-5




//--------------------------------------------------------------------
// Empty classes to give a name to the conditionned graph
//--------------------------------------------------------------------

namespace latticeRCSP{
    
    
//    class ConditionalGraphBuildingResults{
//    public:
//        double maxDifference;
//        int upperBoundNumberOfStates;
//        
//        int nStateVertices;
//        int nStateArcs;
//        double executionBuildingTime;
//        
//        int nVerticesWithStates;
//        int nStateVerticesReachable_withIntraVertexStateArcs;
//        int nStateVerticesReachable_withoutIntraVertexStateArcs;
//        
//        MeetAlgorithmResults meetAlgorithm_withIntraVertexStateArcs;
//        DijkstraLikeHeuristicResults dijsktraHeuristicResults;
//        
//        MeetAlgorithmResults meetAlgorithm_withoutIntraVertexStateArcs;
//        
//    public:
//        ConditionalGraphBuildingResults();
//        void initialize();
//        
//        //--------------------------
//        // Results printing functions for Axel's article
//        //--------------------------
//        void printResults_nonRecursive(ofstream &);
//    };
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
class ConditionalGraph : public GraphWithBounds<Resource, Problem> {
protected:
    //--------------------
    // Conditional Graph
    //--------------------
    
    bool isConditionalGraphInitialized_m;
    
    GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> * conditionedGraph;
    
    bool isConditionnedGraphReverse;
    
    vector<double> conditionalVertexCostLowerBound;
    vector<double> conditionalVertexCostUpperBound;
    
    vector<vector<int> > conditionalVerticesOfVertex;
    vector<int> vertexOfConditionalVertex;
    vector<vector<int> > conditionalArcsOfArc;
    vector<int> arcOfConditionalArc; // -1 if arc of vertex type
    
    ConditionalGraphBuildingResults buildingResults;
    list<ConditionalGraphBuildingResults> previousBuildingResults;
    
    //---------------------
    // Algorithm data structures
    //---------------------
    // acyclic
    vector<multimap<double, int> > networkVertexIncomingStateArcsPerConditionalLB;
    // with cycle
    struct vertexExpansionInformation{
        int arc;
        int precedentState;
        double upperBoundOnAllPathsToStateCost;
        //Remark:
        // In the present version of the algorithm, when an already existing state is updated: the upper bound must be propagated (but if propagated : with its own lower bounds cost)
        // If all arcs costs are positive, this case never happens
    };
    vector<multimap<double, vertexExpansionInformation> > vertexPotentialNewStateMap; //le premiers int est le state vertex et le second int le network arc
    multimap<double,int> verticesToBeExpandedIndexedByMinCostMap;
    vector<multimap<double,int>::iterator> vertexPositionInVerticesToBeExpandedMap;
    
    //vector<double> vertexMaxCost; => replaced by a maxCost and a minCost that are obtained from the GraphWithConditionalBounds 
    
    map<int, Arc<Resource> > stateArcList;
    map<int,int> networkArcOfStateArcList;
    vector<list<int> > stateVerticesOfNetworkVertexList;
    
    map<int,double> stateVertexCostUpperBoundMap;
    map<int,double> stateVertexCostLowerBoundMap;
    
    vector<double> vertexCurrentStateLB;
    vector<double> vertexCurrentStateUB;
    map<int,double> upperBoundsOnAllPathsToStateWhenExpanded;
    
    //vector<list<double>::iterator> vertexCurrentStatePositionInVertexCostUpperBoundList(nVertices);
    vector<int> vertexCurrentState;
    int countStateArcsToBeAdded;

    
public:
    
    //------------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------------
    ConditionalGraph(bool, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> *);
    virtual ~ConditionalGraph();
    
    //------------------------------------------------------------------
    // Getters
    //------------------------------------------------------------------
    vector<double> directionMinConditionnalValueOfSubPath();
    vector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> * > getDirectionConditionalLowerBoundMap();
    void convertPathFromConditionalToConditionned(Path<Resource> *) const;
    bool getIsConditionalGraphInitialized() const;
    
    list<ConditionalGraphBuildingResults> getResultsList();
    
    
    //------------------------------------------------------------------
    // Redefine the direction Lower Bound Algorithm To Use The Conditionnal Bound (in MeetAlgorithm of GraphWithBounds)
    //------------------------------------------------------------------
    virtual vector<Resource *> directionMeetShortestPathAlgorithm();
    virtual void setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();

    //------------------------------------------------------------------
    // ConditionnalGraphBuilding
    //------------------------------------------------------------------
    int upperBoundNumberOfStatesWithDelta(double delta);
    int numberOfNetworkVerticesWithStates();
    
    
    void buildConditionalGraph_acyclic(double delta);
    void buildConditionalGraph_cyclic(double delta);
    void buildConditionalGraph(double delta);
protected:
    //Acylic
    void expandNetworkVertex(int);
    void initializeNetworkVertex(int, double);
    void updateArcsCapacityIfNeeded(int);
    void updateVerticesCapacityIfNeeded(int);

    //nonAcyclic
    void initializeBuildingAlgorithmDataStructures();
    void clearBuildingAlgorithmDataStructures();
    void addSourcesToVerticesToBeExpandedMap();
    void updateVertexByAddingArcsToExistingStateButNoNewState(int vertex, double maxDifference);
    void updateVertexByAddingNewStateAndNewStateArcs(int vertex, double maxDifference);
    void reBuildConditionnalGraphFromAlgorithmDataStructures();
    void initializeConditionalGraph();
    void setOriginDestinationFromConditionnedGraph();
    
};

}


#include "ConditionalGraph.hpp"

#endif /* defined(__RCSP__ConditionalGraph__) */
