//
//  GraphWithConditionalBounds.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 17/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "GraphWithConditionalBounds.h"
#include "../Resources/ResourceFactory.h"

namespace latticeRCSP {
    
    
    
    //-----------------------------
    // Constructor and getters
    //-----------------------------
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::GraphWithConditionalBounds(Graph<Resource> & graph0, Problem * pb) : Graph<Resource>(graph0){
        
        problem_m = pb;
        
        conditionalGraphFromOriginToVertex = new ConditionalGraph<Resource, Problem, ConditionalFunctor>(false, this);
        conditionalGraphFromVertexToDestination = new ConditionalGraph<Resource, Problem, ConditionalFunctor>(true, this);
        
        isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = false;
        isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = false;
        
        isOriginDestinationMaxConditionalValueSet_m = false;
        isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m = false;
        isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m = false;
        
        OriginDestinationMaxConditionalValue = INFINITY;
        
        initializeVertexOriginDestinationType();
        
        Resource r0 = ResourceFactory<Resource>::getStaticNewResource(Graph<Resource>::getIsReverse());
        dummyArcForIntraVertexStateArc = new ArcContext<Resource>("", r0);
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::~GraphWithConditionalBounds(){
        delete conditionalGraphFromVertexToDestination;
        delete conditionalGraphFromOriginToVertex;
        delete dummyArcForIntraVertexStateArc;
        
        ToolsForLatticeRCSP::deleteElementsOfPointersVector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> >(ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex);
        ToolsForLatticeRCSP::deleteElementsOfPointersVector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> >(ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination);

    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    GraphWithConditionalBoundsResults GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getGraphWithConditionalBoundsResults(){
        GraphWithConditionalBoundsResults solution = GraphWithConditionalBoundsResults();
        
        if (isOriginDestinationMaxConditionalValueSet() && isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m && isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m) {
            solution.nActiveVertices = 0;
            solution.maxDistanceFromConditionalLowerBoundToConditionalUpperBound = 0;
            solution.vertexDistanceFromLowerBoundToUpperBound = vector<double>( Graph<Resource>::nVertices, -1);
            
            for (int v = 0; v< Graph<Resource>::nVertices; v++){
                if ((ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex[v] != 0) && (ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination[v] != 0)) {
                    solution.vertexDistanceFromLowerBoundToUpperBound[v] = OriginDestinationMaxConditionalValue - minConditionnalValueOfSubpathFromVertexToDestinationOfSolution[v] - minConditionnalValueOfSubpathFromOriginToVertexOfSolution[v];
                    if (solution.vertexDistanceFromLowerBoundToUpperBound[v] > solution.maxDistanceFromConditionalLowerBoundToConditionalUpperBound) {
                        solution.maxDistanceFromConditionalLowerBoundToConditionalUpperBound = solution.vertexDistanceFromLowerBoundToUpperBound[v];
                    }
                    if (solution.vertexDistanceFromLowerBoundToUpperBound[v] > - EPS) {
                        solution.nActiveVertices++;
                    }
                }
            }
        }
        
        if (!AlgorithmResults::additional_tests) {
            solution.vertexDistanceFromLowerBoundToUpperBound.clear();
        }
        
        return solution;
        
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::initializeVertexOriginDestinationType(){
        set<int> origin;
        set<int> destination;
        
        vertexOriginDestinationType = vector<int>(Graph<Resource>::getNVertices(), 0);
        
        if (Graph<Resource>::getIsReverse()) {
            origin = Graph<Resource>::getDestination();
            destination = Graph<Resource>::getOrigin();
        } else {
            destination = Graph<Resource>::getDestination();
            origin = Graph<Resource>::getOrigin();
        }
        
        for (set<int>::iterator it = origin.begin(); it!= origin.end(); ++it) {
            vertexOriginDestinationType[*it] = 1;
        }
        
        for (set<int>::iterator it = destination.begin(); it!= destination.end(); ++it) {
            vertexOriginDestinationType[*it] = -1;
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::isDestination(int vertex) const{
        if (Graph<Resource>::getIsReverse()) {
            return (vertexOriginDestinationType[vertex] == 1);
        } else{
            return vertexOriginDestinationType[vertex] == -1;
        }
    }
    
    
    //-----------------------------
    // Bounded Graph Domination Functions
    //-----------------------------
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    Problem * GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getProblem() const {
        return problem_m;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::isDominated(int vertex, const Resource & r) const{
        Resource completePath = r;
        if (Graph<Resource>::getIsReverse()) {
            return ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex[vertex]->testCompleteDominated(r, this);
        } else{
            return ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination[vertex]->testCompleteDominated(r, this);
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::isDominated(const Resource &r) const{
        return problem_m->isResourceInfeasibleOrDominatedByASolution(r, problem_m);

    }
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> & pathTested , double & lowerBoundOnCostFromPath) const{
        
        int vertex = pathTested.getDestination();
        if (Graph<Resource>::getIsReverse()) {
//            cout << "this test" << endl;
            return ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex[vertex]->testCompleteDominatedAndUpdateProblemSolution(pathTested, this, lowerBoundOnCostFromPath);
        } else{
            return ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination[vertex]->testCompleteDominatedAndUpdateProblemSolution(pathTested, this, lowerBoundOnCostFromPath);
        }
        
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getIsBoundSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
        } else {
            return isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getIsReverseBoundAndCandidatePathSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
        } else {
            return isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setReverseBoundAndCandidatePath(){
        throw "still not implemented";
    }
    
    //-----------------------------
    // Conditional bounds
    //-----------------------------
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getDirectionIsConditionalBoundSet(){
        if (Graph<Resource>::getIsReverse()) return isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
        else return isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
    }
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setDirectionConditionalGraphsBoundsAndPath(double delta){
        if (Graph<Resource>::getIsReverse()){
            conditionalGraphFromVertexToDestination->buildConditionalGraph(delta);
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> >(ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination);
            ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination = conditionalGraphFromVertexToDestination->getDirectionConditionalLowerBoundMap();
            isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = true;
            isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m = false;
        } else {

            conditionalGraphFromOriginToVertex->buildConditionalGraph(delta);
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> >(ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex);
            ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex = conditionalGraphFromOriginToVertex->getDirectionConditionalLowerBoundMap();
            isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = true;
            isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m = false;
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::addDirectionConditionalPathOfDestinationAsProblemSolution(){
        set<int> destinationSet = Graph<Resource>::getDestination();
        for (set<int>::iterator destinationsIterator = destinationSet.begin(); destinationsIterator != destinationSet.end(); ++destinationsIterator) {
            int vertex = *destinationsIterator;
            vector<Path<Resource > > pathVector;
            if (Graph<Resource>::getIsReverse()) {
                pathVector = ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination[vertex]->getPathVector();
            } else {
                pathVector = ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex[vertex]->getPathVector();
            }
            
            for (int i = 0; i < pathVector.size(); i++) {
//                cout << "Cost " << problem_m->costFunction(pathVector[i].getResource()) << " Constraint " << problem_m->constraintHeuristicKey(pathVector[i].getResource()) << endl;
                if (pathVector[i].getID() != "") problem_m->addPotentialSolutionToSolutionSet(pathVector[i]);
            }
//            cout << "Cost of worst solution in solution set " << *problem_m->getPointerToCostOfWorstSolutionInSolutionSet() << endl;
        }
    }
    
    //-----------------------------
    //Min conditionnal Value of subpath
    //-----------------------------
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::isOriginDestinationMaxConditionalValueSet(){
        return isOriginDestinationMaxConditionalValueSet_m;
    }
    
//    template <typename Resource, typename Problem, typename ConditionalFunctor>
//    double GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::testFeasibilityAndInitializeOriginDestinationMaxConditionalValue(){
//        
//        C'est bien au niveau du problème qu'il fait le mettre pour avoir les types
//        
//    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setOriginDestinationMaxConditionalValue(double value){
        isOriginDestinationMaxConditionalValueSet_m = true;
        OriginDestinationMaxConditionalValue = value;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    double GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getOriginDestinationMaxConditionalValue() const{
        return OriginDestinationMaxConditionalValue;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    double GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getVertexMinConditionnalValue(int vertex){
        if (Graph<Resource>::getIsReverse()) {
//            if (isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m) {
//                <#statements#>
//            }
            return minConditionnalValueOfSubpathFromVertexToDestinationOfSolution[vertex];
        } else {
            return minConditionnalValueOfSubpathFromOriginToVertexOfSolution[vertex];
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    double GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getVertexMaxConditionnalValue(int vertex){
        if (Graph<Resource>::getIsReverse()) {
            return OriginDestinationMaxConditionalValue - minConditionnalValueOfSubpathFromOriginToVertexOfSolution[vertex];
        } else {
            return OriginDestinationMaxConditionalValue - minConditionnalValueOfSubpathFromVertexToDestinationOfSolution[vertex];
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    ArcContext<Resource> * GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getDummyArcContext(){
        return dummyArcForIntraVertexStateArc;
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::updateMinConditionnalValueOfSubPath(){
        if (!isDirectionMinConditionnalValueOfSubPathUpToDate()){
            setDirectionMinConditionnalValueOfSubPath();
        }
        Graph<Resource>::reverseGraph();
        if (!isDirectionMinConditionnalValueOfSubPathUpToDate()){
            setDirectionMinConditionnalValueOfSubPath();
        }
        Graph<Resource>::reverseGraph();
        
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setDirectionMinConditionnalValueOfSubPath(){
        if (getDirectionIsConditionalBoundSet()) {
            setDirectionMinConditionnalValueOfSubPathFromConditionnalGraph();
        } else {
            //cout << "set Dijkstra" << endl;
        	auto logger = Console::getPtrLogger();
        	if (Graph<Resource>::getIsAcyclic()){
        		logger->debug("Set direction min conditional value using acyclic dynamic programming");
        		setDirectionMinConditionnalValueOfSubPathFromAcyclicDynamicProgramming();
        	} else {
        		logger->debug("Set direction min conditional value using Dijkstra algorithm");
        		setDirectionMinConditionnalValueOfSubPathFromDijkstraAlgorithm();
        	}
        }
    }
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::isDirectionMinConditionnalValueOfSubPathUpToDate(){
        if (Graph<Resource>::getIsReverse()) {
            return isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m;
        } else{
            return isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m;
        }
    }

    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setDirectionMinConditionnalValueOfSubPathFromConditionnalGraph(){
        
        dummyArcForIntraVertexStateArc->setResourceToSupremum();
        
        if (Graph<Resource>::getIsReverse()) {
            minConditionnalValueOfSubpathFromVertexToDestinationOfSolution = conditionalGraphFromVertexToDestination->directionMinConditionnalValueOfSubPath();
            isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m = true;
        } else{
            minConditionnalValueOfSubpathFromOriginToVertexOfSolution = conditionalGraphFromOriginToVertex->directionMinConditionnalValueOfSubPath();
            isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m = true;
        }
                
        dummyArcForIntraVertexStateArc->setResourceToNeutralElement();
    }
    
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setDirectionMinConditionnalValueOfSubPathFromDijkstraAlgorithm(){
        if (Graph<Resource>::getIsReverse()) {
            minConditionnalValueOfSubpathFromVertexToDestinationOfSolution = Graph<Resource>::template directionDijkstraAlgorithmToAllVertices<ConditionalFunctor>();
            isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m = true;
        } else {
            minConditionnalValueOfSubpathFromOriginToVertexOfSolution = Graph<Resource>::template directionDijkstraAlgorithmToAllVertices<ConditionalFunctor>();
            isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m = true;
        }
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::setDirectionMinConditionnalValueOfSubPathFromAcyclicDynamicProgramming(){
        if (Graph<Resource>::getIsReverse()) {
            minConditionnalValueOfSubpathFromVertexToDestinationOfSolution = \
            		Graph<Resource>::template directionAcyclicDynamicProgrammingToAllVertices<ConditionalFunctor>();
            isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m = true;
        } else {
            minConditionnalValueOfSubpathFromOriginToVertexOfSolution = \
            		Graph<Resource>::template directionAcyclicDynamicProgrammingToAllVertices<ConditionalFunctor>();
            isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m = true;
        }
    }

    template <typename Resource, typename Problem, typename ConditionalFunctor>
    list<ConditionalGraphBuildingResults> GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getDirectConditionalGraphBuildingResults(){
        return conditionalGraphFromOriginToVertex->getResultsList();
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    list<ConditionalGraphBuildingResults> GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getReverseConditionalGraphBuildingResults(){
        return conditionalGraphFromVertexToDestination->getResultsList();
    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    int GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor>::getSumOfConditionalGraphSize(){
        int result = 0;
        result += max(conditionalGraphFromOriginToVertex->getNVertices(), Graph<Resource>::nVertices);
        result +=  max(conditionalGraphFromVertexToDestination->getNVertices(), Graph<Resource>::nVertices);
        return result;
    }
    
    
}
