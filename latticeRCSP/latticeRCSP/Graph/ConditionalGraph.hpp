
//  ConditionalGraph.cpp
//  RCSP
//
//  Created by Axel Parmentier on 25/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include <cassert>
#include "ConditionalGraph.h"
#include "../Tools/Console.h"

namespace latticeRCSP{
    //------------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------------
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    ConditionalGraph<Resource, Problem, ConditionalFunctor>::ConditionalGraph(bool isReverse, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> *conditioned) : GraphWithBounds<Resource, Problem>(isReverse, conditioned->getProblem()){
        auto logger = Console::getPtrLogger();
        logger->info("Conditional Graph Initialization");
        conditionedGraph = conditioned;
        isConditionnedGraphReverse = isReverse;
        isConditionalGraphInitialized_m = false;
        countStateArcsToBeAdded = -1;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    ConditionalGraph<Resource, Problem, ConditionalFunctor>::~ConditionalGraph(){

    }

    
    //------------------------------------------------------------------
    // Getters
    //------------------------------------------------------------------
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    vector<double> ConditionalGraph<Resource, Problem, ConditionalFunctor>::directionMinConditionnalValueOfSubPath(){
        vector<double> solution(conditionedGraph->getNVertices(), INFINITY);
        
        vector<Resource *> lowerBoundPointers = directionMeetShortestPathAlgorithm();
        
        buildingResults.meetAlgorithm_withoutIntraVertexStateArcs = GraphWithBounds<Resource, Problem>::meetResults;
        buildingResults.nStateVerticesReachable_withoutIntraVertexStateArcs = buildingResults.meetAlgorithm_withoutIntraVertexStateArcs.nbVerticesReachable;
        
        for (int state = 0; state < Graph<Resource>::nVertices; state++) {
            int vertex = vertexOfConditionalVertex[state];
            if (lowerBoundPointers[state] != 0) {
                solution[vertex] = min(solution[vertex], conditionalVertexCostLowerBound[state]);
            }
        }
        
        for (auto && res : lowerBoundPointers){
        	delete res;
        }

        return solution;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(){
        conditionedGraph->getDummyArcContext()->setResourceToSupremum();
        GraphWithBounds<Resource, Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
        conditionedGraph->getDummyArcContext()->setResourceToNeutralElement();
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    vector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> * > ConditionalGraph<Resource, Problem, ConditionalFunctor>::getDirectionConditionalLowerBoundMap(){
        auto logger = Console::getPtrLogger();
        logger->info("--------------------- Conditional graph builds lower bound map --------------------");

        
        
        vector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> * > solution(conditionedGraph->getNVertices(), 0);
        
//        setDirectionMinConditionnalValueOfSubPathFromConditionnalGraph()
        clock_t t0 = clock();
        GraphWithBounds<Resource, Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
//        cout << "1 " << (clock() - t0)/(double) CLOCKS_PER_SEC << endl;
        
        setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
//        cout << "2 " << (clock() - t0)/(double) CLOCKS_PER_SEC << endl;
        
        buildingResults.meetAlgorithm_withIntraVertexStateArcs = GraphWithBounds<Resource, Problem>::meetResults;
        buildingResults.dijsktraHeuristicResults = GraphWithBounds<Resource, Problem>::dijstraHeuristicResults;

        int numberOfActiveStates = 0;
        
//        ProgressBar bar(cout);
//        bar.init();

        for (int vertex = 0; vertex < conditionedGraph->getNVertices(); vertex++) {
//            if (vertex % 500 == 0) {
//                bar.update(vertex, conditionedGraph->getNVertices());
//            }
            multimap<double, pair<Resource, Path<Resource> > > stateOfVertexValues;
            
            for (int s = 0; s < conditionalVerticesOfVertex[vertex].size(); s++) {
                int state = conditionalVerticesOfVertex[vertex][s];
                Resource * bound;
                Path<Resource> * candidate;
                
                if (Graph<Resource>::getIsReverse()) {
                    bound = GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state];
                    candidate = GraphWithBounds<Resource, Problem>::candidatePathFromVertexToDestination[state];
                    
                } else {
                    bound = GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state];
                    candidate = GraphWithBounds<Resource, Problem>::candidatePathFromOriginToVertex[state];
                }
                
                if (bound != 0) {
                    numberOfActiveStates++;
                    if (candidate != 0) {
                        convertPathFromConditionalToConditionned(candidate);
                        stateOfVertexValues.insert(pair<double, pair<Resource, Path<Resource> > >(conditionalVertexCostLowerBound[state], pair<Resource, Path<Resource> >(* bound, * candidate)));
                    }
                    else stateOfVertexValues.insert(pair<double, pair<Resource, Path<Resource> > >(conditionalVertexCostLowerBound[state], pair<Resource, Path<Resource> >(* bound, Path<Resource>(true, -1))));
                    
                }
            }
        

            vector<Resource> bounds(stateOfVertexValues.size());
            vector<Path<Resource> > candidates(bounds.size());
            vector<double> weights(bounds.size());

            int i = 0;
            for (typename multimap<double, pair<Resource, Path<Resource> > >::const_iterator it = stateOfVertexValues.begin(); it != stateOfVertexValues.end(); ++it) {
                weights[i] = it->first;
                bounds[i] = it->second.first;
                candidates[i] = it->second.second;
                i++;
            }
            
            solution[vertex] = new BoundAndPathMap<Resource, Problem, ConditionalFunctor>(weights, bounds, candidates);
        }

        buildingResults.nStateVerticesReachable_withIntraVertexStateArcs = numberOfActiveStates;
        
        logger->info("Number of state vertices reachable: {}", numberOfActiveStates);
        return solution;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool ConditionalGraph<Resource, Problem, ConditionalFunctor>::getIsConditionalGraphInitialized() const{
        return isConditionalGraphInitialized_m;
    }
    
    //------------------------------------------------------------------
    // Redefine the direction Lower Bound Algorithm To Use The Conditionnal Bound (in MeetAlgorithm of GraphWithBounds)
    //------------------------------------------------------------------
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    vector<Resource *> ConditionalGraph<Resource, Problem, ConditionalFunctor>::directionMeetShortestPathAlgorithm(){        
        if (Graph<Resource>::getIsReverse() != conditionedGraph->getIsReverse()) {
            throw "aie";
        }
        if (conditionedGraph->getIsBoundSet()) {
            CostConstraintConditionalBoundMeetAlgorithmFunctor<Resource, Problem, ConditionalFunctor> functorUsingThisGraphBounds(conditionedGraph, vertexOfConditionalVertex);
            return GraphWithBounds<Resource, Problem>::template directionMeetShortestPathAlgorithm<CostConstraintConditionalBoundMeetAlgorithmFunctor<Resource, Problem, ConditionalFunctor> > (functorUsingThisGraphBounds);
        } else {
            NoBoundMeetAlgorithmFunctor<Resource,Problem> noBoundFunctor(this);
            return GraphWithBounds<Resource, Problem>::template directionMeetShortestPathAlgorithm<NoBoundMeetAlgorithmFunctor<Resource,Problem> >(noBoundFunctor);
        }
    }

    //------------------------------------------------------------------
    // ConditionnalGraphBuilding
    //------------------------------------------------------------------

    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::clearBuildingAlgorithmDataStructures()
{
        vertexPotentialNewStateMap.clear();
        verticesToBeExpandedIndexedByMinCostMap.clear();
        vertexPositionInVerticesToBeExpandedMap.clear();
        
        //vector<double> vertexMaxCost; => replaced by a maxCost and a minCost that are obtained from the GraphWithConditionalBounds
        
        stateArcList.clear();
        networkArcOfStateArcList.clear();
        stateVerticesOfNetworkVertexList.clear();
        
        stateVertexCostUpperBoundMap.clear();
        stateVertexCostLowerBoundMap.clear();
        
        vertexCurrentStateLB.clear();
        vertexCurrentStateUB.clear();
        vertexCurrentState.clear();
    }


    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::initializeBuildingAlgorithmDataStructures(){
        
        vertexPotentialNewStateMap = vector<multimap<double,vertexExpansionInformation > >(conditionedGraph->getNVertices()); //le premiers int est le state vertex et le second int le network arc
        vertexPositionInVerticesToBeExpandedMap = vector<multimap<double,int>::iterator>(conditionedGraph->getNVertices());
        //vector<int> vertexNStateVertices(nVertices,0);
//        vertexMaxCost = vector<double>(conditionedGraph->getNVertices(),-1);
        stateVerticesOfNetworkVertexList = vector<list<int> >(conditionedGraph->getNVertices());
       
        vertexCurrentStateLB = vector<double>(conditionedGraph->getNVertices(),-INFINITY);
        vertexCurrentStateUB = vector<double>(conditionedGraph->getNVertices(),-INFINITY);
        vertexCurrentState = vector<int>(conditionedGraph->getNVertices(),-1);
        
        Graph<Resource>::nVertices = 0;
        Graph<Resource>::nArcs = 0;
        
        countStateArcsToBeAdded = 0;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::addSourcesToVerticesToBeExpandedMap(){
        for (set<int>::iterator it = conditionedGraph->getOrigin().begin(); it!=  conditionedGraph->getOrigin().end(); ++it) {
            int vertex = *it;
            
            vertexExpansionInformation sourceInformation;
            sourceInformation.arc = -1;
            sourceInformation.precedentState = -1;
            sourceInformation.upperBoundOnAllPathsToStateCost = 0;
            
            vertexPotentialNewStateMap[vertex].insert(pair<double, vertexExpansionInformation>(0,sourceInformation));
            verticesToBeExpandedIndexedByMinCostMap.insert(pair<double,int>(0,vertex));
            
            //Graph<Resource>::origin().insert(Graph<Resource>::nVertices);
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::updateVertexByAddingArcsToExistingStateButNoNewState(int vertex, double maxDifference){
        
        //---------------------------------------------
        // Indentify the cost smaller than last state UB
        //---------------------------------------------

        typename multimap<double, vertexExpansionInformation >::iterator it = vertexPotentialNewStateMap[vertex].upper_bound(vertexCurrentStateLB[vertex] + maxDifference + EPS);
        
        //---------------------------------------------
        // Update last state UB
        //---------------------------------------------
        --it;
        if (it->first > vertexCurrentStateUB[vertex]) {
            vertexCurrentStateUB[vertex] = it->first;
            stateVertexCostUpperBoundMap.erase(vertexCurrentState[vertex]);
            stateVertexCostUpperBoundMap.insert(pair<int, double>(vertexCurrentState[vertex],vertexCurrentStateUB[vertex]));
        }
        if (it->first < vertexCurrentStateLB[vertex]){
            throw "Algorithms works only with non-negative ConditionalFunctor";
        }
        
        ++it;
        
        //---------------------------------------------
        // Build the Arcs
        //---------------------------------------------
        
        double maxCostOfAllPathsToState = - INFINITY;

        
        for (typename multimap<double,vertexExpansionInformation>::iterator iter = vertexPotentialNewStateMap[vertex].begin(); iter != it; ++iter) {
            
        	assert(!(iter->first < vertexCurrentStateLB[vertex])); //  "This part of the code is not right: indeed, the value used fot the state vertex lower bounds is probably false" << endl;
            
            int arc = iter->second.arc;
            int stateOrigin = iter->second.precedentState;
            if (stateOrigin != -1){
                countStateArcsToBeAdded--;
                ArcContext<Resource> const * pointer = conditionedGraph->getArc(arc).getArcContext();
                stateArcList.insert(pair<int, Arc<Resource> >(Graph<Resource>::nArcs, Arc<Resource>(pointer , stateOrigin, vertexCurrentState[vertex], Graph<Resource>::getIsReverse())));
                networkArcOfStateArcList.insert(pair<int, int>(Graph<Resource>::nArcs,arc));
                
                //cout << "stateArc " << nArcs <<  " oristate " << stateOrigin  <<" destState " << vertexCurrentState[vertex] << " network arc " << arc << " Origin state lower bound + arc cost " << iter->first << " LB"  << vertexCurrentStateLB[vertex] << " UB " << vertexCurrentStateUB[vertex] << "paf" << endl;
                
                Graph<Resource>::nArcs++;
            }
        }
        
        
        map<int,double>::iterator upperBoundIterator = upperBoundsOnAllPathsToStateWhenExpanded.find(vertexCurrentState[vertex]);
        if (maxCostOfAllPathsToState > upperBoundIterator->second) {
            
            
            auto logger = Console::getPtrLogger();
            logger->critical("This part of the code is not right: indeed, the value used fot the state vertex lower bounds is probably false");
            throw "This part of the code is not right: indeed, the value used fot the state vertex lower bounds is probably false";
            
            upperBoundsOnAllPathsToStateWhenExpanded.erase(upperBoundIterator);
            
            //--------------------------------------------------------
            //-------------- Expand the state vertex
            //--------------------------------------------------------
            upperBoundsOnAllPathsToStateWhenExpanded.insert(pair<int, double>(Graph<Resource>::nVertices, maxCostOfAllPathsToState));
            
            
            for (int a = 0; a < conditionedGraph->deltaPa(vertex).size(); a++) {
                int arc = conditionedGraph->deltaPa(vertex)[a];
                int newVertex = conditionedGraph->arcDestination(arc);
                double newVertexCost = vertexCurrentStateLB[vertex] + ConditionalFunctor::getKey(conditionedGraph->getArc(arc).getResource());
                double newMaxCostOfAllPathsToState = maxCostOfAllPathsToState + ConditionalFunctor::getKey(conditionedGraph->getArc(arc).getResource());
                
                //------------------------
                // Test if it can be a new State
                //------------------------
                //            if (true){
                
//                double minCondValue = conditionedGraph->getVertexMinConditionnalValue(newVertex);
//                double macCondValue = conditionedGraph->getVertexMaxConditionnalValue(newVertex);
                
                
                if ((conditionedGraph->getVertexMinConditionnalValue(newVertex) < newMaxCostOfAllPathsToState + EPS ) && (newVertexCost < conditionedGraph->getVertexMaxConditionnalValue(newVertex) + EPS) ) {
//                if (newVertexCost < conditionedGraph->getVertexMaxConditionnalValue(newVertex) + EPS){
                    countStateArcsToBeAdded++;
                    if (vertexPotentialNewStateMap[newVertex].empty()) {
                        vertexPositionInVerticesToBeExpandedMap[newVertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(newVertexCost,newVertex));
                    } else if (newVertexCost < vertexPotentialNewStateMap[newVertex].begin()->first) {
                        verticesToBeExpandedIndexedByMinCostMap.erase(vertexPositionInVerticesToBeExpandedMap[newVertex]);
                        vertexPositionInVerticesToBeExpandedMap[newVertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(newVertexCost,newVertex));
                    }
                    
                    vertexExpansionInformation information;
                    information.arc = arc;
                    information.precedentState = Graph<Resource>::nVertices;
                    information.upperBoundOnAllPathsToStateCost = newMaxCostOfAllPathsToState;
                    
                    vertexPotentialNewStateMap[newVertex].insert(pair<double, vertexExpansionInformation>(newVertexCost,information));
                }
            }
        }
        
        
        //---------------------------------------------
        // Erase the updated cost from the cost to be update map
        //---------------------------------------------
        vertexPotentialNewStateMap[vertex].erase(vertexPotentialNewStateMap[vertex].begin(), it);
        if (!vertexPotentialNewStateMap[vertex].empty()) {
            vertexPositionInVerticesToBeExpandedMap[vertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(vertexPotentialNewStateMap[vertex].begin()->first,vertex));
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::updateVertexByAddingNewStateAndNewStateArcs(int vertex, double maxDifference){
        // get the cost which must be expanded

        double vertexCostLB = vertexPotentialNewStateMap[vertex].begin()->first;
        typename multimap<double,vertexExpansionInformation>::iterator it = vertexPotentialNewStateMap[vertex].upper_bound(vertexCostLB + maxDifference);
        --it;
        double vertexCostUB = it->first;
        ++it;
        
        double maxCostOfAllPathsToState = -INFINITY;
        
        //--------------------------------------------------------
        //-------------- Build the state arcs and erase the corresponding costs
        //--------------------------------------------------------
        for (typename multimap<double,vertexExpansionInformation>::iterator iter = vertexPotentialNewStateMap[vertex].begin(); iter != it; ++iter) {
            int arc = iter->second.arc;
            int stateOrigin = iter->second.precedentState;
            
            // used to cut in thanks to lower bounds
            maxCostOfAllPathsToState = max(maxCostOfAllPathsToState, iter->second.upperBoundOnAllPathsToStateCost);
            
            if (stateOrigin != -1){
                countStateArcsToBeAdded--;
                stateArcList.insert(pair<int,Arc<Resource> >(Graph<Resource>::nArcs, Arc<Resource>(conditionedGraph->getArc(arc).getArcContext(), stateOrigin,Graph<Resource>::nVertices, Graph<Resource>::getIsReverse())));
                networkArcOfStateArcList.insert(pair<int, int>(Graph<Resource>::nArcs, arc));
                
                //cout << "stateArc " << nArcs <<  " oristate " << stateOrigin  <<" destState " << nVertices << " network arc " << arc << " Origin state lower bound + arc cost " << iter->first << " LB"  << vertexCostLB << " UB " << vertexCostUB << endl;
                
                Graph<Resource>::nArcs++;
            }
        }
        //Erase the costs
        vertexPotentialNewStateMap[vertex].erase(vertexPotentialNewStateMap[vertex].begin(), it);
        if (!vertexPotentialNewStateMap[vertex].empty()) {
            vertexPositionInVerticesToBeExpandedMap[vertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(vertexPotentialNewStateMap[vertex].begin()->first,vertex));
        }
        
        
        // Arc between two successive state vertices on the same vertex
        if (!stateVerticesOfNetworkVertexList[vertex].empty()) {
            list<int>::iterator inclusionArc = stateVerticesOfNetworkVertexList[vertex].end();
            --inclusionArc;
            stateArcList.insert(pair<int, Arc<Resource> >(Graph<Resource>::nArcs,Arc<Resource>(conditionedGraph->getDummyArcContext() , *inclusionArc, Graph<Resource>::nVertices, Graph<Resource>::getIsReverse())));
            networkArcOfStateArcList.insert(pair<int, int>(Graph<Resource>::nArcs, -1));
            Graph<Resource>::nArcs++;
        }
        
        
        
        //--------------------------------------------------------
        //-------------- Build the state vertex
        //--------------------------------------------------------
        stateVertexCostUpperBoundMap.insert(pair<int, double>(Graph<Resource>::nVertices,vertexCostUB));
        stateVertexCostLowerBoundMap.insert(pair<int, double>(Graph<Resource>::nVertices,vertexCostLB));
        vertexCurrentStateLB[vertex]=vertexCostLB;
        vertexCurrentStateUB[vertex]=vertexCostUB;
        vertexCurrentState[vertex] = Graph<Resource>::nVertices;
        
        stateVerticesOfNetworkVertexList[vertex].push_back(Graph<Resource>::nVertices);

        
        //--------------------------------------------------------
        //-------------- Expand the state vertex
        //--------------------------------------------------------
        upperBoundsOnAllPathsToStateWhenExpanded.insert(pair<int, double>(Graph<Resource>::nVertices, maxCostOfAllPathsToState));

        
        for (int a = 0; a < conditionedGraph->deltaPa(vertex).size(); a++) {
            int arc = conditionedGraph->deltaPa(vertex)[a];
            int newVertex = conditionedGraph->arcDestination(arc);
            double newVertexCost = vertexCostLB + ConditionalFunctor::getKey(conditionedGraph->getArc(arc).getResource());
            double newMaxCostOfAllPathsToState = maxCostOfAllPathsToState + ConditionalFunctor::getKey(conditionedGraph->getArc(arc).getResource());
            
            //------------------------
            // Test if it can be a new State
            //------------------------
//            if (true){
            
//            double minCondValue = conditionedGraph->getVertexMinConditionnalValue(newVertex);
//            double macCondValue = conditionedGraph->getVertexMaxConditionnalValue(newVertex);
            
            
//            if (newVertexCost < conditionedGraph->getOriginDestinationMaxConditionalValue()){
            
//            if ((conditionedGraph->getVertexMinConditionnalValue(newVertex) < newMaxCostOfAllPathsToState + EPS ) &&(newVertexCost < conditionedGraph->getOriginDestinationMaxConditionalValue())){

            if ((conditionedGraph->getVertexMinConditionnalValue(newVertex) < newMaxCostOfAllPathsToState + EPS ) && (newVertexCost < conditionedGraph->getVertexMaxConditionnalValue(newVertex) + EPS) ) {
            
//            if (newVertexCost < conditionedGraph->getVertexMaxConditionnalValue(newVertex) + EPS){
                countStateArcsToBeAdded++;
                if (vertexPotentialNewStateMap[newVertex].empty()) {
                    vertexPositionInVerticesToBeExpandedMap[newVertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(newVertexCost,newVertex));
                } else if (newVertexCost < vertexPotentialNewStateMap[newVertex].begin()->first) {
                    verticesToBeExpandedIndexedByMinCostMap.erase(vertexPositionInVerticesToBeExpandedMap[newVertex]);
                    vertexPositionInVerticesToBeExpandedMap[newVertex] = verticesToBeExpandedIndexedByMinCostMap.insert(pair<double, int>(newVertexCost,newVertex));
                }
                
                vertexExpansionInformation information;
                information.arc = arc;
                information.precedentState = Graph<Resource>::nVertices;
                information.upperBoundOnAllPathsToStateCost = newMaxCostOfAllPathsToState;
                
                vertexPotentialNewStateMap[newVertex].insert(pair<double, vertexExpansionInformation>(newVertexCost,information));
            }
        }
        
        
        Graph<Resource>::nVertices++;
    
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::reBuildConditionnalGraphFromAlgorithmDataStructures(){
        //----------------------------------------
        // Convert the list of arc into a vector
        //----------------------------------------
        if (stateVertexCostLowerBoundMap.size() != stateVertexCostUpperBoundMap.size()) throw "pb";
        Graph<Resource>::Arcs = vector<Arc<Resource> >(Graph<Resource>::nArcs);
        arcOfConditionalArc = vector<int>(Graph<Resource>::nArcs);
        conditionalVertexCostUpperBound = vector<double>(Graph<Resource>::nVertices);
        conditionalVertexCostLowerBound = vector<double>(Graph<Resource>::nVertices);
        typename map<int, Arc<Resource> >::iterator itArcs = stateArcList.begin();
        map<int,int>::iterator itNetArc = networkArcOfStateArcList.begin();
        for (int a = 0; a < Graph<Resource>::nArcs; a++) {
            Graph<Resource>::Arcs[itArcs->first] = itArcs->second;
            //Graph<Resource>::setArc(itArcs->first, itArcs->second.first, itArcs->second.second);
            //        stateArcs[itArcs->first][0] = itArcs->second.first;
            //        stateArcs[itArcs->first][1] = itArcs->second.second;
            arcOfConditionalArc[itNetArc->first] = itNetArc->second;
            ++itArcs;
            ++itNetArc;
        }
        if (!(itArcs == stateArcList.end()) || !(itNetArc == networkArcOfStateArcList.end())) {
            throw "pb";
        }
        
        conditionalVerticesOfVertex = vector<vector<int> >(conditionedGraph->getNVertices());
        for (int v= 0; v<conditionedGraph->getNVertices(); v++) {
            conditionalVerticesOfVertex[v] = vector<int>(stateVerticesOfNetworkVertexList[v].size());
            int i = 0;
            for (list<int>::iterator it = stateVerticesOfNetworkVertexList[v].begin(); it != stateVerticesOfNetworkVertexList[v].end(); ++it) {
                conditionalVerticesOfVertex[v][i] = *it;
                i++;
            }
        }
        
        map<int,double>::iterator itUB = stateVertexCostUpperBoundMap.begin();
        map<int,double>::iterator itLB = stateVertexCostLowerBoundMap.begin();
        
        for (int s = 0; s<Graph<Resource>::nVertices; s++) {
            conditionalVertexCostLowerBound[itLB->first] = itLB->second;
            conditionalVertexCostUpperBound[itUB->first] = itUB->second;
            ++itLB;
            ++itUB;
        }
        if (itUB != stateVertexCostUpperBoundMap.end() || itLB != stateVertexCostLowerBoundMap.end()) {
            throw "pb";
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::initializeConditionalGraph(){
        //----------------------
        // Initialize State graph attributes
        //----------------------
        vertexOfConditionalVertex = vector<int>(Graph<Resource>::nVertices);
        for (int v = 0; v<conditionedGraph->getNVertices(); v++) {
            for (int s = 0; s < conditionalVerticesOfVertex[v].size(); s++) {
                vertexOfConditionalVertex[conditionalVerticesOfVertex[v][s]] = v;
            }
        }
        
        //---------------------
        // Initialize the graph
        //---------------------
        //setRecursiveArcID();
        setOriginDestinationFromConditionnedGraph();
        Graph<Resource>::initialize();
        
//        // Initialize the Resource
//        Graph<Resource>::Resources = vector<Resource>(Graph<Resource>::nArcs);
//        for (int a = 0; a<Graph<Resource>::nArcs; a++) {
//            if (arcOfConditionalArc[a] != -1) Graph<Resource>::Resources[a] = conditionedGraph->getArcResource(arcOfConditionalArc[a]);
//            else {
//                Graph<Resource>::Resources[a] = Resource();
//                Graph<Resource>::Resources[a].setCost(0);
//            }
//        }
        
        isConditionalGraphInitialized_m = true;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::setOriginDestinationFromConditionnedGraph(){
        set<int> conditionnedOrigin = conditionedGraph->getOrigin();
        set<int> conditionnedDestination = conditionedGraph->getDestination();
        
        set<int> newOrigin;
        for (set<int>::iterator it = conditionnedOrigin.begin(); it != conditionnedOrigin.end(); ++it) {
            for (unsigned int s = 0; s < conditionalVerticesOfVertex[*it].size(); s++) {
                if (conditionalVertexCostLowerBound[conditionalVerticesOfVertex[*it][s]] == 0) {
                    newOrigin.insert(conditionalVerticesOfVertex[*it][s]);
                }
            }
        }
        
//        for (set<int>::iterator it = conditionnedOrigin.begin(); it != conditionnedOrigin.end(); ++it) {
//            for (unsigned int s = 0; s < conditionalVerticesOfVertex[*it].size(); s++) {
//                newOrigin.insert(conditionalVerticesOfVertex[*it][s]);
//            }
//        }
        
        set<int> newDestination;
        for (set<int>::iterator it = conditionnedDestination.begin(); it != conditionnedDestination.end(); ++it) {
            for (unsigned int s = 0; s < conditionalVerticesOfVertex[*it].size(); s++) {
                newDestination.insert(conditionalVerticesOfVertex[*it][s]);
            }
        }
        
        Graph<Resource>::setOriginDestination_withDirection(newOrigin,newDestination);
        
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::buildConditionalGraph_acyclic(double maxDifference){
    	clock_t t0 = clock();
    	auto logger = Console::getPtrLogger();
		logger->info("---------------------- Build conditional graph ----------------------");
		if (!conditionedGraph->getIsAcyclic()){
			logger->critical("conditioned graph must be acyclic");
			throw;
		}
		Graph<Resource>::isAcyclic = true;
		isConditionnedGraphReverse = Graph<Resource>::getIsReverse();

		if (Graph<Resource>::getIsReverse() != conditionedGraph->getIsReverse()) {
			conditionedGraph->reverseGraph();
		}
		if (! conditionedGraph->isOriginDestinationMaxConditionalValueSet()) {
			logger->info("Origin Destination Max ConDitionnal Value Is Not Set -- I set them now");
			pair<bool, Path<Resource> > sol = conditionedGraph->getProblem()->testProblemFeasibilityAndGetFeasibleResource();
			if (sol.first){
				conditionedGraph->setOriginDestinationMaxConditionalValue(ConditionalFunctor::getKey(sol.second.getResource()));
			} else {
				logger->critical("Problem is Unfeasible");
				throw "Problem is Unfeasible";
			}
		}
		logger->info("Update minimum conditional value of subpath");
		conditionedGraph->updateMinConditionnalValueOfSubPath();

		// Upper Bound Number of states + build structures
		networkVertexIncomingStateArcsPerConditionalLB = vector<multimap<double, int> >(conditionedGraph->getNVertices());
		int upperBoundNumberOfStateVertices = 0;
		int upperBoundNumberOfStateArcs = 0;
		conditionalVerticesOfVertex = vector<vector<int> >(conditionedGraph->getNVertices());
		conditionalArcsOfArc = vector<vector<int> >(conditionedGraph->getNArcs());
		for (int v = 0; v < conditionedGraph->getNVertices(); v++) {
			int vertexUB = ceil((conditionedGraph->getVertexMaxConditionnalValue(v) - conditionedGraph->getVertexMinConditionnalValue(v)) / maxDifference);
			if (vertexUB < 0) vertexUB = 0; //Doit etre mis a jour (permet de detecter des infaisabilites)
//			conditionalVerticesOfVertex[v].reserve(vertexUB);
			upperBoundNumberOfStateVertices += vertexUB;
			upperBoundNumberOfStateArcs += vertexUB * (1 + conditionedGraph->deltaPa(v).size());
//			for (auto && arc : conditionedGraph->deltaPa(v)){
//				conditionalArcsOfArc.reserve(vertexUB);
//			}
		}
		upperBoundNumberOfStateVertices = min(upperBoundNumberOfStateVertices, 10 * conditionedGraph->getNVertices());
		upperBoundNumberOfStateArcs = min(upperBoundNumberOfStateArcs, 10 * conditionedGraph->getNArcs());
		conditionalVertexCostLowerBound.reserve(upperBoundNumberOfStateVertices);
		conditionalVertexCostUpperBound.reserve(upperBoundNumberOfStateVertices);
		vertexOfConditionalVertex.reserve(upperBoundNumberOfStateVertices);
		Graph<Resource>::Arcs.reserve(upperBoundNumberOfStateArcs);
		arcOfConditionalArc.reserve(upperBoundNumberOfStateArcs);

		// Initialize origin
		for (auto && v : conditionedGraph->getOrigin()){
			Graph<Resource>::addOrigin(Graph<Resource>::nVertices);
			conditionalVertexCostLowerBound.push_back(0);
			conditionalVertexCostUpperBound.push_back(0);
			conditionalVerticesOfVertex[v].push_back(Graph<Resource>::nVertices);
			vertexOfConditionalVertex.push_back(v);
			expandNetworkVertex(v);
			++Graph<Resource>::nVertices;
		}

		// Initialize other vertices
		for (int vert = 0; vert < conditionedGraph->getNVertices(); ++vert){
			int v = conditionedGraph->topologicalOrder(vert);
//		for (auto && v : conditionedGraph->getTopologicalOrder()){
			if (conditionalVerticesOfVertex[v].size() > 0) continue; // origin
			initializeNetworkVertex(v, maxDifference);
			expandNetworkVertex(v);
		}

		// Initialize Destinations
		for (auto && v : conditionedGraph->getDestination()){
			for (auto && s : conditionalVerticesOfVertex[v]){
				Graph<Resource>::addDestination(s);
			}
		}
		Graph<Resource>::setIsOriginDestinationSet(true);

		// Free memory unused
		networkVertexIncomingStateArcsPerConditionalLB.clear();
		networkVertexIncomingStateArcsPerConditionalLB.shrink_to_fit();
		for (auto && vec : conditionalVerticesOfVertex){
			vec.shrink_to_fit();
		}
		for (auto && vec : conditionalArcsOfArc){
			vec.shrink_to_fit();
		}
		conditionalVertexCostLowerBound.shrink_to_fit();
		conditionalVertexCostUpperBound.shrink_to_fit();
		vertexOfConditionalVertex.shrink_to_fit();
		Graph<Resource>::Arcs.shrink_to_fit();
		arcOfConditionalArc.shrink_to_fit();

		// Initialize Graph
		Graph<Resource>::initializeDelta();
		Graph<Resource>::initializeSourceAndSink();
		Graph<Resource>::initializeCyclicOrTopologicalOrder();
		isConditionalGraphInitialized_m = true;
		logger->info("Conditional graph has {} vertices and {} arcs", Graph<Resource>::nVertices, Graph<Resource>::nArcs);
		logger->info("Conditional graph building time {} s", (clock() - t0)/(double) CLOCKS_PER_SEC);
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
	void ConditionalGraph<Resource, Problem, ConditionalFunctor>::updateArcsCapacityIfNeeded(int v){
    	if (Graph<Resource>::nArcs == Graph<Resource>::Arcs.capacity()){
    		int newCapa = ceil(1.5 *(double) \
    				Graph<Resource>::nArcs *(double) \
					conditionedGraph->getNVertices() \
					/(double) (max(conditionedGraph->topologicalOrderOfV(v), 1)));
    		Graph<Resource>::Arcs.reserve(newCapa);
    		arcOfConditionalArc.reserve(newCapa);
    	}
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::updateVerticesCapacityIfNeeded(int v){
    	if (Graph<Resource>::nVertices == vertexOfConditionalVertex.capacity()){
			int newCapa = ceil(1.5 *(double) \
					Graph<Resource>::nVertices *(double) \
					conditionedGraph->getNVertices() \
					/(double) (max(conditionedGraph->topologicalOrderOfV(v), 1)));
			vertexOfConditionalVertex.reserve(newCapa);
			conditionalVertexCostLowerBound.reserve(newCapa);
			conditionalVertexCostUpperBound.reserve(newCapa);
    	}
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::expandNetworkVertex(int vertex_l){
    	updateArcsCapacityIfNeeded(vertex_l);
    	for (auto && netArc : conditionedGraph->deltaPa(vertex_l)){
    		auto pArcContext = conditionedGraph->getArc(netArc).getArcContext();
    		double arcCost = ConditionalFunctor::getKey(pArcContext->getResource());
    		int netDest = conditionedGraph->arcDestination(netArc);
    		conditionalArcsOfArc[netArc].reserve(conditionalVerticesOfVertex[vertex_l].size());
    		for (auto && staVer : conditionalVerticesOfVertex[vertex_l]){
    			double newMinCost = conditionalVertexCostLowerBound[staVer] + arcCost;
    			if (newMinCost > conditionedGraph->getVertexMaxConditionnalValue(netDest)) break;

    			conditionalArcsOfArc[netArc].push_back(Graph<Resource>::nArcs);
    			arcOfConditionalArc.push_back(netArc);
    			Graph<Resource>::Arcs.push_back(Arc<Resource>(pArcContext, \
    					staVer, -1, Graph<Resource>::getIsReverse()));

    			networkVertexIncomingStateArcsPerConditionalLB[netDest].insert(\
    					pair<double, int>(newMinCost, Graph<Resource>::nArcs));

    			++Graph<Resource>::nArcs;
    		}
    	}
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::initializeNetworkVertex(int netVert, double maxDifference){
    	conditionalVerticesOfVertex[netVert].reserve(networkVertexIncomingStateArcsPerConditionalLB[netVert].size());
    	bool first = true;
    	while (!networkVertexIncomingStateArcsPerConditionalLB[netVert].empty()){
    		if (first) first = false;
    		else {
    			arcOfConditionalArc.push_back(-1);
    			Graph<Resource>::Arcs.push_back(Arc<Resource>(conditionedGraph->getDummyArcContext(), \
    			    					Graph<Resource>::nVertices -1, Graph<Resource>::nVertices, Graph<Resource>::getIsReverse()));
    			++Graph<Resource>::nArcs;
    		}
    		updateVerticesCapacityIfNeeded(netVert);
    		double lb = networkVertexIncomingStateArcsPerConditionalLB[netVert].begin()->first;
    		conditionalVertexCostLowerBound.push_back(lb);
    		auto endIt = networkVertexIncomingStateArcsPerConditionalLB[netVert].upper_bound(lb + maxDifference);
			--endIt;
    		conditionalVertexCostUpperBound.push_back(endIt->first);
    		++endIt;

    		for(auto it = networkVertexIncomingStateArcsPerConditionalLB[netVert].begin(); it != endIt; ++it){
    			Graph<Resource>::Arcs[it->second].setDestination(Graph<Resource>::nVertices, Graph<Resource>::getIsReverse());
    		}
    		vertexOfConditionalVertex.push_back(netVert);
    		conditionalVerticesOfVertex[netVert].push_back(Graph<Resource>::nVertices);
    		++Graph<Resource>::nVertices;
    		networkVertexIncomingStateArcsPerConditionalLB[netVert].erase( \
    				networkVertexIncomingStateArcsPerConditionalLB[netVert].begin(), endIt);
    	}
    	conditionalVerticesOfVertex[netVert].shrink_to_fit();
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
	void ConditionalGraph<Resource, Problem, ConditionalFunctor>::buildConditionalGraph(double maxDifference){
    	if (conditionedGraph->getIsAcyclic()) buildConditionalGraph_acyclic(maxDifference);
    	else buildConditionalGraph_cyclic(maxDifference);
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::buildConditionalGraph_cyclic(double maxDifference){
//        throw "aie";
        auto logger = Console::getPtrLogger();
        logger->info("---------------------- Build conditional graph ----------------------");

        
        //----------------------------
        // Initialization
        //----------------------------
        if (Graph<Resource>::getIsReverse() != conditionedGraph->getIsReverse()) {
            conditionedGraph->reverseGraph();
        }
        if (! conditionedGraph->isOriginDestinationMaxConditionalValueSet()) {
            logger->info("Origin Destination Max ConDitionnal Value Is Not Set -- I set them now");
            pair<bool, Path<Resource> > sol = conditionedGraph->getProblem()->testProblemFeasibilityAndGetFeasibleResource();
            if (sol.first){
                conditionedGraph->setOriginDestinationMaxConditionalValue(ConditionalFunctor::getKey(sol.second.getResource()));
            } else {
                logger->critical("Problem is Unfeasible");
                throw "Problem is Unfeasible";
            }

        }
        logger->info("Update minimum conditional value of subpath");

        conditionedGraph->updateMinConditionnalValueOfSubPath();
        
        previousBuildingResults.push_back(buildingResults);
        buildingResults.initialize();
        
        int upperBoundOnNumberOfStates = upperBoundNumberOfStatesWithDelta(maxDifference);
//
//        cout << "Algo starts ----" << endl;
//        cout << "Conditional graph building: " << endl;
//
        clock_t t0 = clock();
        initializeBuildingAlgorithmDataStructures();

        //----------------------------
        // Algorithm
        //----------------------------
//        ProgressBar progress_bar(cout);
//        progress_bar.init();
        addSourcesToVerticesToBeExpandedMap();
        bool isThereAStateOnDestination = false;
        logger->info("|Stat built|Max states|");

        int nIterations =0;
        while (!verticesToBeExpandedIndexedByMinCostMap.empty()) {
            if (nIterations % 5000 == 0) {
            	logger->info("|{:>9} |{:>9} |", Graph<Resource>::nVertices, upperBoundOnNumberOfStates);
//                progress_bar.update(Graph<Resource>::nVertices, upperBoundOnNumberOfStates);
            }
//            throw "aie";
            // get the expanded vertex and erase it
            int vertex = verticesToBeExpandedIndexedByMinCostMap.begin()->second;
            
            if (conditionedGraph->isDestination(vertex)) {
                isThereAStateOnDestination = true;
            }
            
            verticesToBeExpandedIndexedByMinCostMap.erase(verticesToBeExpandedIndexedByMinCostMap.begin());
            
            // Test if a new state must be added or if only arcs towards the current state must be added
            if (vertexPotentialNewStateMap[vertex].begin()->first < vertexCurrentStateLB[vertex] + maxDifference + EPS){
                updateVertexByAddingArcsToExistingStateButNoNewState(vertex, maxDifference);

            }
            else { //build the new state
                updateVertexByAddingNewStateAndNewStateArcs(vertex, maxDifference);

            }

        }
        
#ifdef DEBUG_TESTS
        for (int v = 0; v<conditionedGraph->getNVertices(); v++) {
            if (vertexPotentialNewStateMap[v].size()>0){
                throw "aie";
            }
        }
#endif

        //----------------------------
        // Post-Treatment
        //----------------------------
        if (!isThereAStateOnDestination){
            logger->info("No feasible state: optimal solution found");
            throw true;
        }
        
        reBuildConditionnalGraphFromAlgorithmDataStructures();

        this->clearBuildingAlgorithmDataStructures();
        
        
        buildingResults.maxDifference = maxDifference;
        buildingResults.upperBoundNumberOfStates = upperBoundOnNumberOfStates;
        
        buildingResults.nStateVertices = Graph<Resource>::nVertices;
        buildingResults.nStateArcs = Graph<Resource>::nArcs;
        buildingResults.executionBuildingTime = clock() - t0;
        if (AlgorithmResults::additional_tests) {
            buildingResults.nVerticesWithStates = numberOfNetworkVerticesWithStates();
        }
        

        logger->info(" number of state vertices: {}", Graph<Resource>::nVertices);
        logger->info(" number of state arcs: {}", Graph<Resource>::nArcs);

        logger->info("Initialize conditional graph ----");
        initializeConditionalGraph();
//        cout << endl << "Set direction meet lower bound ----" << endl;
//        GraphWithBounds<Resource, Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        logger->info("End of conditional graph building ----");

    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void ConditionalGraph<Resource, Problem, ConditionalFunctor>::convertPathFromConditionalToConditionned(Path<Resource> * path0) const{
        path0->setOrigin_neverReverse(vertexOfConditionalVertex[path0->getOrigin_neverReverse()]);
        path0->setDestination_neverReverse(vertexOfConditionalVertex[path0->getDestination_neverReverse()]);

    }
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    int ConditionalGraph<Resource, Problem, ConditionalFunctor>::upperBoundNumberOfStatesWithDelta(double delta){
        int solution = 0;
        for (int v = 0; v < conditionedGraph->getNVertices(); v++) {
            solution += ceil((conditionedGraph->getVertexMaxConditionnalValue(v) - conditionedGraph->getVertexMinConditionnalValue(v)) / delta);
        }
        return solution;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    int ConditionalGraph<Resource, Problem, ConditionalFunctor>::numberOfNetworkVerticesWithStates(){
        int solution = 0;
        for (int v = 0; v< conditionedGraph->getNVertices(); v++) {
            if (stateVerticesOfNetworkVertexList[v].begin() != stateVerticesOfNetworkVertexList[v].end()) {
                solution++;
            }
        }
        return solution;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    list<ConditionalGraphBuildingResults> ConditionalGraph<Resource, Problem, ConditionalFunctor>::getResultsList(){
        list<ConditionalGraphBuildingResults> solutionResults = previousBuildingResults;
        solutionResults.push_back(buildingResults);
        solutionResults.pop_front();
        return solutionResults;
    }
    
//    template <typename Resource, typename ConditionalFunctor, typename Problem>
//    template<typename goodPathSetKeyFunctor>
//    vector<BoundAndPathMap<Resource, ConditionalFunctor, Problem> * > ConditionalGraph<Resource, Problem, ConditionalFunctor>::getBoundSet() {
//        vector<BandPSetMap<boundAndPaths<Resource>, ConditionalFunctor> > result(conditionedGraph->getNVertices());
//        for (int vertex = 0; vertex < conditionedGraph->getNVertices(); vertex++) {
//            int nStates = conditionalVerticesOfVertex[vertex].size();
//            vector<double> weights(nStates);
//            vector<boundAndPaths<Resource> > boundSets(nStates);
//            for (int s = 0; s < nStates; s++) {
//                int state = conditionalVerticesOfVertex[vertex][s];
//                weights[s] = conditionalVertexCostLowerBound[state];
//                boundSets[s] = boundAndPaths<Resource>(boundAndPaths<Resource>(Graph<Resource>::getLowerBoundOnPathFromOriginToVertex(state), Graph<Resource>::template getSetOfGoodPathsFromOriginToVertex<goodPathSetKeyFunctor>(state)));
//
////                boundSets[s] = boundAndPaths<Resource>(Graph<Resource>::getLowerBoundOnPathFromVertexToDestination(state), Graph<Resource>::getSetOfGoodPathsFromVertexToDestination<goodPathSetKeyFunctor>(state));
//            }
//            result[vertex] = BandPSetMap<boundAndPaths<Resource>, ConditionalFunctor>(weights, boundSets);
//        }
//        return result;
//    }
//    
//    template <typename Resource, typename ConditionalFunctor, typename networkGraphType>
//    template<typename argumentBoundSet>
//    vector<BandPSetMap<argumentBoundSet, ConditionalFunctor> > ConditionalGraph<Resource, Problem, ConditionalFunctor>::getBoundSet(vector<argumentBoundSet> argumentSetVector) const{
//        vector<BandPSetMap<argumentBoundSet, ConditionalFunctor> > result(conditionedGraph->getNVertices());
//        for (int vertex = 0; vertex < conditionedGraph->getNVertices(); vertex++) {
//            int nStates = conditionalVerticesOfVertex[vertex].size();
//            vector<double> weights(nStates);
//            vector<argumentBoundSet> boundSets(nStates);
//            for (int s = 0; s < nStates; s++) {
//                int state = conditionalVerticesOfVertex[vertex][s];
//                weights[s] = conditionalVertexCostLowerBound[state];
//                boundSets[s] = argumentSetVector[state];
//            }
//            result[vertex] = BandPSetMap<boundAndPaths<Resource>, ConditionalFunctor>(weights, boundSets);
//        }
//        return result;
//    }
//
//    template <typename Resource, typename ConditionalFunctor, typename networkGraphType>
//    int ConditionalGraph<Resource, Problem, ConditionalFunctor>::getNetworkArcRecursive(int arc) const{
//        return conditionedGraph->getNetworkArcRecursive(arcOfConditionalArc[arc]);
//    }
//    
//    template <typename Resource, typename ConditionalFunctor, typename networkGraphType>
//    void  ConditionalGraph<Resource, Problem, ConditionalFunctor>::setRecursiveArcID(){
//        Graph<Resource>::isArcIDset = true;
//        Graph<Resource>::arcID = vector<int>(graph::nArcs);
//        for (int arc = 0; arc < graph::nArcs; arc++) {
//            Graph<Resource>::arcID[arc] = getNetworkArcRecursive(arc);
//        }
//    }
//    
}
