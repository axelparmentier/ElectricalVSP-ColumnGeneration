//
//  ResourceGraph.cpp
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "GraphWithBounds.h"
#include "../Resources/ResourceFactory.h"
#include "MeetAlgorithmFunctors/MeetAlgorithmConstraintHeuristicFunctor.h"
#include "../Tools/Console.h"
//#define DESACTIVATE_CANDIDATE_PATHS

namespace latticeRCSP {
    
    //--------------------------
    // Constructors
    //--------------------------
    
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem>::GraphWithBounds() : Graph<Resource>(){
        problem_m = 0;
        initializeBooleans();
        
    }
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem>::GraphWithBounds(bool isReverse, Problem * problem0) : Graph<Resource>(isReverse){
        problem_m = problem0;
        initializeBooleans();
    }
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem>::GraphWithBounds(Graph<Resource> const * graph0, Problem * problem0) : Graph<Resource>(graph0){
        problem_m = problem0;
        initializeBooleans();
    }
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem>::GraphWithBounds(Graph<Resource> & graph0, Problem * problem0) : Graph<Resource>(graph0){
        problem_m = problem0;
        initializeBooleans();
    }
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem>::~GraphWithBounds(){
        ToolsForLatticeRCSP::deleteElementsOfPointersVector<Resource>(LowerBoundOnProblemSolutionSubPathsFromOriginToVertex);
        ToolsForLatticeRCSP::deleteElementsOfPointersVector<Resource>(LowerBoundOnProblemSolutionSubPathsFromVertexToDestination);

        ToolsForLatticeRCSP::deleteElementsOfPointersVector<Path<Resource> >(candidatePathFromOriginToVertex);
        ToolsForLatticeRCSP::deleteElementsOfPointersVector<Path<Resource> >(candidatePathFromVertexToDestination);
    }
    
    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::initializeBooleans(){
        isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = false;
        isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = false;
        
        isCandidatePathFromOriginToVertexSet_m = false;
        iscandidatePathFromVertexToDestinationSet_m = false;
        
        meetResults = MeetAlgorithmResults();
        dijstraHeuristicResults = DijkstraLikeHeuristicResults();
    }
    
    //-------------------------------
    // Getters and setters
    //-------------------------------
    template <typename Resource, typename Problem>
    Problem * GraphWithBounds<Resource, Problem>::getProblem() const{
        return problem_m;
    }
    
    template <typename Resource, typename Problem>
    Resource GraphWithBounds<Resource, Problem>::getBoundFromOriginToVertex(int vertex) const{
        return *LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex];
    }
    
    template <typename Resource, typename Problem>
    Resource GraphWithBounds<Resource, Problem>::getBoundFromVertexToDestination(int vertex) const{
        return *LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex];
    }
    
    
    template <typename Resource, typename Problem>
    MeetAlgorithmResults GraphWithBounds<Resource, Problem>::getMeetResults() const {
       return meetResults;
    }


    template <typename Resource, typename Problem>
    DijkstraLikeHeuristicResults GraphWithBounds<Resource, Problem>::getDijkstraHeuristicResults() const {
       return dijstraHeuristicResults;
    }
    //-----------------------------------------------------------------------------
    // Meet Algorithm for lower bounds
    //-----------------------------------------------------------------------------
    
    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm(){
        if (! Graph<Resource>::getIsOriginAndDestinationSet()) {
        	auto logger = Console::getPtrLogger();
        	logger->warn("Origin and destination not set in meet algorithm. Set source and sink as origin and destination");
            Graph<Resource>:: setSourceAndSinkAsOriginAndDestination();
        }
            
        if (!Graph<Resource>::getIsReverse()) {
//            isLatestBoundBuiltReverse_m = false;
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<Resource>(LowerBoundOnProblemSolutionSubPathsFromOriginToVertex);
            LowerBoundOnProblemSolutionSubPathsFromOriginToVertex = directionMeetShortestPathAlgorithm();
            //throw "aie";
            isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = true;
        } else {
//            isLatestBoundBuiltReverse_m = true;
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<Resource>(LowerBoundOnProblemSolutionSubPathsFromVertexToDestination);
            LowerBoundOnProblemSolutionSubPathsFromVertexToDestination = directionMeetShortestPathAlgorithm();
            isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = true;
        }
    }
    
    template <typename Resource, typename Problem>
    vector<Resource *> GraphWithBounds<Resource, Problem>::directionMeetShortestPathAlgorithm(){
        if (getIsBoundSet()) {
            CostConstraintSimpleBoundMeetAlgorithmFunctor<Resource,Problem> functorUsingThisGraphBounds(this);
        	auto logger = Console::getPtrLogger();
        	logger->info("Reverse bounds are already set when calling meet algorithm. You compute bounds twice. I use them to discard vertex when computing bounds in this direction.");
            return directionMeetShortestPathAlgorithm<CostConstraintSimpleBoundMeetAlgorithmFunctor<Resource,Problem> >(functorUsingThisGraphBounds);
        } else {
            NoBoundMeetAlgorithmFunctor<Resource,Problem> noBoundFunctor(this);
        	auto logger = Console::getPtrLogger();
            logger->debug("Meet algorithm uses no bound functors to discard paths (standard choice)");
            return directionMeetShortestPathAlgorithm<NoBoundMeetAlgorithmFunctor<Resource,Problem> >(noBoundFunctor);
        }
    }
    
    template <typename Resource, typename Problem>
    template <typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> GraphWithBounds<Resource, Problem>::directionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & functor){
        
        meetResults.initialize();
        meetResults.isAcyclic = Graph<Resource>::isAcyclic;
        
        if (Graph<Resource>::isAcyclic) {
            return acyclicDirectionMeetShortestPathAlgorithm<isMeetDominatedAtVertexFunctor>(functor);
        } else {
            return withCycleDirectionMeetShortestPathAlgorithm<isMeetDominatedAtVertexFunctor>(functor);
        }
    }
    
    template <typename Resource, typename Problem>
    template <typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> GraphWithBounds<Resource, Problem>::acyclicDirectionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & functor){
    	auto logger = Console::getPtrLogger();
	   logger->info("--------------------------       Acyclic Meet Algorithm        --------------------------");
        
        clock_t t0 = clock();
        
        set<int> origin = Graph<Resource>::getOrigin();
        vector<Resource *> vertexMeetResource(Graph<Resource>::nVertices, 0);
        for (set<int>::iterator it = origin.begin(); it!=origin.end(); ++it) {
            int vertex = *it;
            delete vertexMeetResource[vertex];
            vertexMeetResource[vertex] = ResourceFactory<Resource>::getPtrNewResource(Graph<Resource>::getIsReverse());
        }
        
//        ProgressBar bar(cout);
//        bar.init();
//
        logger->info("|Vert treat| Vert tot|");
        
        for (int i = 0; i<Graph<Resource>::nVertices; i++) {
            if (i % 10000 == 0) {
            	logger->info("|{:>9} |{:>9} |", i, Graph<Resource>::nVertices);
//            	bar.update(i, Graph<Resource>::nVertices);
            }
            
            int v = Graph<Resource>::topologicalOrder(i);
            if (Graph<Resource>::vertexType(v) != 1) {
                for (int a = 0; a <Graph<Resource>::deltaMa(v).size(); a++) {
//                    vector<int> deltaM = Graph<Resource>::deltaMa(v);
                    int arc = Graph<Resource>::deltaMa(v)[a];
                    if (vertexMeetResource[Graph<Resource>::arcOrigin(arc)] != 0){
                        Resource expandedResource = * vertexMeetResource[Graph<Resource>::arcOrigin(arc)];
                        directionalExpand(expandedResource,Graph<Resource>::Arcs[arc].getResource());
//                        expandedResource.expand(Graph<Resource>::Arcs[arc].getResource());
                        if (! (functor.getKey(v,expandedResource))) {
                            if (vertexMeetResource[v] == 0) {
                                vertexMeetResource[v] = ResourceFactory<Resource>::getPtrNewResource(Graph<Resource>::getIsReverse());
                                vertexMeetResource[v]->setToSupremum();
                            }
                            vertexMeetResource[v]->meet(expandedResource);
                            meetResults.nbUpdates++;
                        }
                    }
                }
            }
            if (vertexMeetResource[v] == 0){
                meetResults.nbVerticesReachable++;
            }
        }
        
        
        meetResults.nbExpansions = meetResults.nbVerticesReachable;
        meetResults.executionTime = clock() - t0;
        logger->info("Acyclic meet execution time (s): {}", (clock() - t0)/(double) CLOCKS_PER_SEC);

        
        return vertexMeetResource;
    }
    
    template <typename Resource, typename Problem>
    typename GraphWithBounds<Resource, Problem>::MeetAlgorithmFunctor GraphWithBounds<Resource, Problem>::meetAlgorithmFunctor_m = GraphWithBounds<Resource, Problem>::MEET_FUNCTOR_CONSTRAINT_HEURISTIC;

    template <typename Resource, typename Problem>
    template <typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> GraphWithBounds<Resource, Problem>::withCycleDirectionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & functor){
        if (meetAlgorithmFunctor_m == MEET_FUNCTOR_PATH_LENGTH){
           MeetAlgorithmPathLengthFunctor<Resource>::setMaxLength(Graph<Resource>::nVertices);
           return withCycleDirectionMeetShortestPathAlgorithmWithMeetKeyFunctor<isMeetDominatedAtVertexFunctor ,\
           MeetAlgorithmPathLengthFunctor<Resource> >(functor);
        } else if (meetAlgorithmFunctor_m == MEET_FUNCTOR_CONSTRAINT_HEURISTIC){
           MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem>::setProblem(problem_m);
           return withCycleDirectionMeetShortestPathAlgorithmWithMeetKeyFunctor<isMeetDominatedAtVertexFunctor ,\
              MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem> >(functor);
        } else {
        	auto logger = Console::getPtrLogger();
        	logger->error("Unknown meet algorithm functor");
           throw;
        }

    }
    
    template <typename Resource, typename Problem>
    template<typename isMeetDominatedAtVertexFunctor, typename VertexResourceKeyFunctor>
    vector<Resource *> GraphWithBounds<Resource, Problem>::withCycleDirectionMeetShortestPathAlgorithmWithMeetKeyFunctor(isMeetDominatedAtVertexFunctor & functor){
    	auto logger = Console::getPtrLogger();
    	logger->info("--------------------------       Cyclic Meet Algorithm        --------------------------");
        
        clock_t t0 = clock();
        
        // Initialization -----------------------
        vector<Resource *> vertexMeetResource(Graph<Resource>::nVertices,0);
        vector<VertexResourceKeyFunctor> vertexInformation(Graph<Resource>::nVertices, VertexResourceKeyFunctor());
        multimap<double, int> mapVertexToBeExpanded;
        
        int nbExpansion = 0;
        int nbUpdate = 0;
        int nbVertexExpanded= 0;
        vector<bool> vertexExpanded(Graph<Resource>::nVertices, false);
        
        
        // Origin vertex -------------------
        set<int> origin = Graph<Resource>::getOrigin();
        for (set<int>::iterator it = origin.begin(); it!=origin.end(); ++it) {
//            cout << "origin inserted" << endl;
            int vertex = *it;
            delete vertexMeetResource[vertex];
            vertexMeetResource[vertex] =ResourceFactory<Resource>::getPtrNewResource(Graph<Resource>::getIsReverse());
            vertexInformation[vertex].setPathLength(0);
            vertexInformation[vertex].setVertexPositionInMap(mapVertexToBeExpanded.insert(pair<double, int>(\
                        vertexInformation[vertex].getKey(),vertex)));
        }
        
        // Algorithm
//        ProgressBar bar(cout);
//        bar.init();
//
        logger->info("| Vert exp |queue size| Vert tot|");
        
        while (!mapVertexToBeExpanded.empty()) {
            map<double,int>::iterator it = mapVertexToBeExpanded.begin();
            nbExpansion++;
            
            if (nbExpansion % 1000 == 0) {
            	logger->info("|{:>9} |{:>9} |{:>9} |", nbVertexExpanded, mapVertexToBeExpanded.size(), Graph<Resource>::nVertices);
//                bar.update(nbVertexExpanded, Graph<Resource>::nVertices + mapVertexToBeExpanded.size());
            }

            
            
            int expandedVertex = it->second;
            
            if (!vertexExpanded[expandedVertex]) nbVertexExpanded++;
            vertexExpanded[expandedVertex] = true;
            
            // expand
            for (int a = 0; a< Graph<Resource>::deltaPa(expandedVertex).size(); a++) {
                nbUpdate++;
                int arc = Graph<Resource>::deltaPa(expandedVertex)[a];
                int updatedVertex = Graph<Resource>::arcDestination(arc);
                updateVertexFromArc<isMeetDominatedAtVertexFunctor, VertexResourceKeyFunctor>(arc,\
                                    vertexMeetResource[expandedVertex], \
                                    vertexInformation[expandedVertex], \
                                    vertexMeetResource[updatedVertex],   \
                                    vertexInformation[updatedVertex], \
                                    mapVertexToBeExpanded, \
                                    functor);
            }
            
            // Erase
            mapVertexToBeExpanded.erase(it);
            vertexInformation[expandedVertex].removedFromMap();
        }
        
        meetResults.nbExpansions = nbExpansion;
        meetResults.nbUpdates = nbUpdate;
        meetResults.nbVerticesReachable = nbVertexExpanded;
        meetResults.executionTime = clock() - t0;
        
        logger->info("|  nb upd  |  nb exp  | Vert exp | time (s) |");
        logger->info("|{:>9} |{:>9} |{:>9} |{:>9} |", nbUpdate, nbExpansion, nbVertexExpanded, (clock() - t0)/(float) CLOCKS_PER_SEC);
        
        return vertexMeetResource;
    }
    
    template <typename Resource, typename Problem>
    template <typename isMeetDominatedAtVertexFunctor, typename VertexResourceKeyFunctor>
    void GraphWithBounds<Resource, Problem>::updateVertexFromArc(int arc, Resource * expandedVertexResource, VertexResourceKeyFunctor & expandedVertexInformation, Resource * & updatedVertexResource,  VertexResourceKeyFunctor & updatedVertexInformation, multimap<double,int> & vertexToBeExpanded, isMeetDominatedAtVertexFunctor & functor){

        // Determine the updating information
        int updatedVertex = Graph<Resource>::arcDestination(arc);
        Resource updatingResource = *expandedVertexResource;
//        updatingResource.expand(Graph<Resource>::Arcs[arc].getArcContext()->getResource());
        directionalExpand(updatingResource,Graph<Resource>::Arcs[arc].getArcContext()->getResource());
        
        // Update only if non dominated
        if (!functor.getKey(updatedVertex, updatingResource)) {
            if (updatedVertexResource == 0) {
                updatedVertexResource = ResourceFactory<Resource>::getPtrNewResource(Graph<Resource>::getIsReverse());
                updatedVertexResource->setToSupremum();
            }
            if (!updatedVertexResource->isLeq(updatingResource)) {
                //
                updatedVertexResource->meet(updatingResource);
            // Update Vertex information
            
                short int isUpdatedAndShouldBeDeleted = updatedVertexInformation.update(expandedVertexInformation, updatedVertexResource);
                if (isUpdatedAndShouldBeDeleted == 2) {
                    vertexToBeExpanded.erase(updatedVertexInformation.getVertexPositionInMap());
                }
                
                // Insert in the map of vertices to be expanded
                if (isUpdatedAndShouldBeDeleted > 0) {
                    updatedVertexInformation.setVertexPositionInMap(vertexToBeExpanded.insert(pair<double, int>(updatedVertexInformation.getKey(),updatedVertex)));
                }
                
            }
        }
    }
    
    //-----------------------------------------------------------------------------
    // Algorithm to compute good paths set for upper bound
    //-----------------------------------------------------------------------------
    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(){
#ifndef DESACTIVATE_CANDIDATE_PATHS
        ProblemOfGraphHeuristicKeyFunctor<Resource, Problem>::setGraph(this);
        ProblemOfGraphHeuristicKeyFunctor<Resource, Problem>::lockGraph();
        
        setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm<ProblemOfGraphHeuristicKeyFunctor<Resource, Problem> >();
        
        ProblemOfGraphHeuristicKeyFunctor<Resource, Problem>::unlockGraph();
#endif
    }
    
    template <typename Resource, typename Problem>
    template<typename heuristicToFindGoodPathsMinimizedFunctor>
    void GraphWithBounds<Resource, Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(){
#ifndef DESACTIVATE_CANDIDATE_PATHS
       if (! Graph<Resource>::getIsOriginAndDestinationSet()) {
       	auto logger = Console::getPtrLogger();
       	logger->warn("Origin and destination not set in good paths heuristic algorithm. Set source and sink as origin and destination");
//            cout << "Set source and sink as origin and destination" << endl;
            Graph<Resource>:: setSourceAndSinkAsOriginAndDestination();
        }
        
        if (!Graph<Resource>::getIsReverse()) {
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<Path<Resource> >(candidatePathFromOriginToVertex);
            candidatePathFromOriginToVertex = directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm<heuristicToFindGoodPathsMinimizedFunctor>();
            isCandidatePathFromOriginToVertexSet_m = true;
        } else {
            ToolsForLatticeRCSP::deleteElementsOfPointersVector<Path<Resource> >(candidatePathFromVertexToDestination);
            candidatePathFromVertexToDestination = directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm<heuristicToFindGoodPathsMinimizedFunctor>();
            iscandidatePathFromVertexToDestinationSet_m = true;
        }
#endif
    }
    
    
    template <typename Resource, typename Problem>
    template<typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > GraphWithBounds<Resource, Problem>::directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(){
        if (getIsBoundSet()) {
            CostConstraintSimpleBoundMeetAlgorithmFunctor<Resource,Problem> functorUsingThisGraphBounds(this);
            return directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm<CostConstraintSimpleBoundMeetAlgorithmFunctor<Resource,Problem>,  heuristicToFindGoodPathsMinimizedFunctor>(functorUsingThisGraphBounds);
        } else {
            NoBoundMeetAlgorithmFunctor<Resource,Problem> noBoundFunctor(this);
            return directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm \
                    <NoBoundMeetAlgorithmFunctor<Resource,Problem> ,  heuristicToFindGoodPathsMinimizedFunctor> \
                    (noBoundFunctor);
        }
    }
    
    template <typename Resource, typename Problem>
    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > GraphWithBounds<Resource, Problem>::directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor & functor){
        
        dijstraHeuristicResults.initialize();
        dijstraHeuristicResults.isAcyclic = Graph<Resource>::isAcyclic;
        
        if (Graph<Resource>::isAcyclic) {
            return acyclicDirectionDijkstraLikeAlgorithm<isPathDominatedAtVertexFunctor,  heuristicToFindGoodPathsMinimizedFunctor>(functor);
        } else {
            return withCycleDirectionDijkstraLikeAlgorithm<isPathDominatedAtVertexFunctor,  heuristicToFindGoodPathsMinimizedFunctor>(functor);
        }
    }
    
    template <typename Resource, typename Problem>
    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > GraphWithBounds<Resource, Problem>::acyclicDirectionDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor & functor){
    	auto logger = Console::getPtrLogger();
    	logger->info("---------------   Acyclic heuristic algorithm to find good paths    --------------------");

        clock_t t0 = clock();
        
        set<int> origin = Graph<Resource>::getOrigin();
        vector<Path<Resource> *> vertexPathReturned(Graph<Resource>::nVertices, 0);
        for (set<int>::iterator it = origin.begin(); it!=origin.end(); ++it) {
            int vertex = *it;
            delete vertexPathReturned[vertex];
            vertexPathReturned[vertex] = new Path<Resource>(Graph<Resource>::getIsReverse(), vertex);
        }
        
//        ProgressBar bar(cout);
//        bar.init();
        
        logger->info("|Vert treat| Vert tot |");
        for (int i = 0; i<Graph<Resource>::nVertices; i++) {
            if (i% 10000 == 0) {
//                bar.update(i, Graph<Resource>::nVertices);
            	logger->info("|{:>9} |{:>9} |", i, Graph<Resource>::nVertices);
            }
            
            int vertex = Graph<Resource>::topologicalOrder(i);
            if (Graph<Resource>::vertexType(vertex) != 1) {
                for (int a = 0; a <Graph<Resource>::deltaMa(vertex).size(); a++) {
                    //vector<int> deltaM = Graph<Resource>::deltaMa(v);
                    int arc = Graph<Resource>::deltaMa(vertex)[a];
                    if (vertexPathReturned[Graph<Resource>::arcOrigin(arc)] != 0){
                        Path<Resource> * expandedPath = new Path<Resource>(* vertexPathReturned[Graph<Resource>::arcOrigin(arc)]);
                        expandedPath->expand(Graph<Resource>::Arcs[arc]);
                        Resource expandedPathResource = expandedPath->getResource();
                        if (! (functor.getKey(vertex, expandedPathResource))) {
                            //problem_m->addPotentialSolutionToSolutionSet(*expandedPath);
                            if (vertexPathReturned[vertex] == 0 || \
                                heuristicToFindGoodPathsMinimizedFunctor::getKey(* expandedPath) < \
                                heuristicToFindGoodPathsMinimizedFunctor::getKey(* vertexPathReturned[vertex])) {
                                delete vertexPathReturned[vertex];
                                vertexPathReturned[vertex] = expandedPath;
                            } else delete expandedPath;
                        } else delete expandedPath;
                    }
                }
            }
            if (vertexPathReturned[vertex] != 0) {
                dijstraHeuristicResults.nbVerticesWithUpperBound++;
            }
        }
        
        dijstraHeuristicResults.executionTime = clock() - t0;
        logger->info("Acyclic heuristic execution time (s): {} ", (clock() - t0)/(double) CLOCKS_PER_SEC);

        
        return vertexPathReturned;
    }
    
    template <typename Resource, typename Problem>
    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > GraphWithBounds<Resource, Problem>::withCycleDirectionDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor & functor){
    	auto logger = Console::getPtrLogger();
    	logger->info("---------------    Cyclic heuristic algorithm to find good paths    --------------------");

        clock_t t0 = clock();
        
        vector<Path<Resource> *> vertexPathReturned(Graph<Resource>::nVertices, 0);
        vector<bool> insertedVertices(Graph<Resource>::nVertices, false);
        vector<double> shortestPathToVertex(Graph<Resource>::nVertices, INFINITY);
        
        multimap<double,int> candidateVertices; // Vertices candidate for entrance
        
        set<int> origin = Graph<Resource>::getOrigin();
        for (set<int>::iterator it = origin.begin(); it!=origin.end(); ++it) {
            int vertex = *it;
            delete vertexPathReturned[vertex];
            vertexPathReturned[vertex] = new Path<Resource>(Graph<Resource>::getIsReverse(), vertex);
            candidateVertices.insert(pair<double, int>(0,vertex));
            //insertedVertices[vertex] = true;
            shortestPathToVertex[vertex] = INFINITY;
        }
        
        
        int countUpdatedVertices = 0;
//        ProgressBar bar(cout);
//        bar.init();
        logger->info("| Vert upd | Vert tot |");

        while (!candidateVertices.empty()) {
            if (countUpdatedVertices % 10000 == 0 ) {
            	logger->info("|{:>9} |{:>9} |", countUpdatedVertices, Graph<Resource>::nVertices);
//            	bar.update(countUpdatedVertices, Graph<Resource>::nVertices);
            }
            // get the nearest neighboor
            multimap<double, int>::iterator beg = candidateVertices.begin();
            int currentVertex = beg->second;
//            double currentCost = beg->first;
            candidateVertices.erase(beg);
            
            if (!insertedVertices[currentVertex]) { // if vertex is not in the active subgraph, add the vertex
                // update the descendants
                insertedVertices[currentVertex] = true;
                for (int a =0; a < Graph<Resource>::deltaPa(currentVertex).size(); a++) {
                    int arc = Graph<Resource>::deltaPa(currentVertex)[a];
                    int newVertex = Graph<Resource>::arcDestination(arc);
                                        
                    //cout << "current vertex " << currentVertex << " arc " << arc <<  " newVertex " << newVertex << endl;
                    
                    if (!insertedVertices[newVertex]) {
                        Path<Resource> * expandedPath = new Path<Resource>(* vertexPathReturned[currentVertex]);
                        expandedPath->expand(Graph<Resource>::Arcs[arc]);
                        Resource expandedPathResource = expandedPath->getResource();

                        if (! (functor.getKey(newVertex,expandedPathResource))) {
                            double heuristicValue = heuristicToFindGoodPathsMinimizedFunctor::getKey(* expandedPath);
                            if (heuristicValue < shortestPathToVertex[newVertex]){
                                shortestPathToVertex[newVertex] = heuristicValue;
//                                if (vertexPathReturned[newVertex] != 0){
                                   delete vertexPathReturned[newVertex];
//                                }
                                vertexPathReturned[newVertex] = expandedPath;
                                candidateVertices.insert(pair<double, int>(heuristicValue,newVertex));
                            } else {
                                //cout << "larger heuristic" << endl;
                                delete expandedPath;
                            }
                        } else {
                            //cout << "non Feasible path" << endl;
                            delete expandedPath;
                        }
                    } //else cout << "Already inserted vertex updated " << newVertex << endl;
                }
            } //else cout << "Already inserted vertex treated" << endl;
        }
        
        for (int v = 0; v<vertexPathReturned.size(); v++) {
            if (vertexPathReturned[v] != 0) {
                dijstraHeuristicResults.nbVerticesWithUpperBound++;
            }
        }
        
        return vertexPathReturned;
    }

    
    //-----------------------------------------------------------------------------
    // Tests on bounds and paths
    //-----------------------------------------------------------------------------

    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::isDominated(int vertex, const Resource & r) const{
        Resource completePath = r;
        if (Graph<Resource>::getIsReverse()) {
            if (LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex] == 0) {
                // Vertex is not reachable
                return true;
            }
            directionalExpand(completePath,*LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex]);
//            completePath.expand(*LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex]);
        } else {
            if(LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex] == 0){
                return true;
            }
            directionalExpand(completePath,* LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex]);
//            completePath.expand(* LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex]);
        }
        return problem_m->isResourceInfeasibleOrDominatedByASolution(completePath);
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::isDominated(const Resource & r) const{
        return problem_m->isResourceInfeasibleOrDominatedByASolution(r);
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::isDominated(const Path<Resource> & p, double & lowerBoundOnCostFromLabel) const{
        Resource completePath = p.getResource();
#ifdef KEY_WITHOUT_BOUND
        lowerBoundOnCostFromLabel = problem_m->costFunction(completePath);
#endif
        int vertex = p.getDestination();
        if (p.isReverse()) {
            if (LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex] == 0) {
                // Vertex is not reachable
                return true;
            }
            directionalExpand(completePath,*LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex]);
//            completePath.expand(*LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[vertex]);
        } else {
            if(LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex] == 0){
                return true;
            }
            directionalExpand(completePath,*LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex]);
//            completePath.expand(* LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[vertex]);
        }
#ifndef KEY_WITHOUT_BOUND
        lowerBoundOnCostFromLabel = problem_m->costFunction(completePath);
#endif
        return problem_m->isResourceInfeasibleOrDominatedByASolution(completePath);
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::isPartialDominated(const Path<Resource> & p, double & lowerBoundOnCostFromLabel) const{
       Resource partialPath = p.getResource();
       lowerBoundOnCostFromLabel = problem_m->costFunction(partialPath);
       return problem_m->isResourceInfeasibleOrDominatedByASolution(partialPath);
    }

    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::getIsBoundSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
        } else {
            return isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
        }

    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::getIsGoodPathSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isCandidatePathFromOriginToVertexSet_m;
        } else {
            return iscandidatePathFromVertexToDestinationSet_m;
        }
        
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::getIsReverseBoundAndCandidatePathSet() const{
        if (Graph<Resource>::getIsReverse()) {
            return isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m && isCandidatePathFromOriginToVertexSet_m;
        } else {
            return isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m && iscandidatePathFromVertexToDestinationSet_m;
        }
        
    }
    
    template <typename Resource, typename Problem>
     bool GraphWithBounds<Resource, Problem>::getIsReverseCandidatePathSet() const{
         if (Graph<Resource>::getIsReverse()) {
             return isCandidatePathFromOriginToVertexSet_m;
         } else {
             return iscandidatePathFromVertexToDestinationSet_m;
         }

     }

    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::setReverseBoundAndCandidatePath(){
        Graph<Resource>::reverseGraph();
        setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
        Graph<Resource>::reverseGraph();
    }
    
    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::setReverseCandidatePath(){
        Graph<Resource>::reverseGraph();
        setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
        Graph<Resource>::reverseGraph();
    }

    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::testPathFeasibility(string pathID){//, Resource initialResource){
        
        int oldNumSolSearched = problem_m->getNumberOfSolutionsSearched();
        problem_m->setNumberOfSolutionsSearched(oldNumSolSearched +1);

        
        list<string> arcList;
        while (pathID.find('_') != string::npos){
            size_t pos = pathID.find("_",1);
            arcList.push_back(pathID.substr(0,pos));
            if (pathID.find('_') != string::npos) pathID.erase(0,pos+1);
            else pathID.erase(0,pos);
        }
        arcList.push_back(pathID);
        
        
        int vertex = -1;
        int arc = -1;
        list<string>::iterator stringIter;
        stringIter = arcList.begin();
        
        set<int>::const_iterator oriter;
        
        for (oriter = Graph<Resource>::getOrigin().begin(); oriter !=  Graph<Resource>::getOrigin().end(); ++oriter) {
            if (findArc(*stringIter, *oriter) != -1){
                arc = *oriter;
            }
        }
        
        if (arc == -1) {
            cerr << "unable to initialize in GraphWithBounds<Resource, Problem>::testPathFeasibility(string pathID)" << endl;
            throw;
        }
        vertex = Graph<Resource>::arcOrigin(arc);
        
        Resource pathResource = Resource();
        for (stringIter = arcList.begin();stringIter != arcList.end(); ++stringIter){
            arc = findArc(*stringIter, vertex);
            if (arc == -1) {
                cerr << "unable to find arc in GraphWithBounds<Resource, Problem>::testPathFeasibility(string pathID)" << endl;
                throw;
            }
            pathResource.expand(Graph<Resource>::Arcs[arc].getArcContext()->getResource());
            vertex = Graph<Resource>::arcDestination(arc);

            //Test Feasibility
            if (isDominated(vertex, pathResource)) {
                return false;
            }
        }
        
        return true;
    }
    
    template <typename Resource, typename Problem>
    int GraphWithBounds<Resource, Problem>::findArc(string arcName, int vertex){
        vector<int>::const_iterator arcIt;
        for (arcIt = Graph<Resource>::deltaPa(vertex).begin(); arcIt != Graph<Resource>::deltaPa(vertex).end(); ++arcIt){
            if (Graph<Resource>::Arcs[*arcIt].getArcContext()->getID() == arcName) {
                return *arcIt;
            }
        }
        return -1;
    }
    
    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::directionalExpand(Resource & resourceExtended, const Resource & resourceAdded) const{
    	directionalExpand(resourceExtended, resourceAdded, Graph<Resource>::getIsReverse());
    }

    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::directionalExpand(Resource & resourceExtended, const Resource & resourceAdded, bool isReverse){
    	if (isReverse){
    		Resource copyResource = resourceExtended;
    		resourceExtended = resourceAdded;
    		resourceExtended.expand(copyResource);
    	} else {
    		resourceExtended.expand(resourceAdded);
    	}
    }

    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::addToPotentialSolutionSetIfAtDestination(const Path<Resource> & pathTested) const{
       int destination = pathTested.getDestination();
       if (Graph<Resource>::getConstDestination().find(destination) != Graph<Resource>::getConstDestination().end()){
          problem_m->addPotentialSolutionToSolutionSet(pathTested);
       }
    }

    template <typename Resource, typename Problem>
    void GraphWithBounds<Resource, Problem>::completePathAndAddToPotentialSolutionSet(const Path<Resource> & pathTested) const{
        Path<Resource> expandedPath = pathTested;
        int vertex = expandedPath.getDestination();
        if (Graph<Resource>::getIsReverse()){
        	if (candidatePathFromOriginToVertex[vertex] != 0 ){
	            expandedPath.expand(*(candidatePathFromOriginToVertex[vertex]));
				problem_m->addPotentialSolutionToSolutionSet(expandedPath);
        	}
        } else if (candidatePathFromVertexToDestination[vertex] != 0){
        	expandedPath.expand(*(candidatePathFromVertexToDestination[vertex]));
            problem_m->addPotentialSolutionToSolutionSet(expandedPath);
        }
    }
    
    template <typename Resource, typename Problem>
    bool GraphWithBounds<Resource, Problem>::testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> & pathTested, double & lowerBoundOnCostFromLabel) const{
        if (!isDominated(pathTested, lowerBoundOnCostFromLabel)){
#ifdef DESACTIVATE_CANDIDATE_PATHS
           addToPotentialSolutionSetIfAtDestination(pathTested);
#else
            completePathAndAddToPotentialSolutionSet(pathTested);
#endif
            return false;
        } else return true;
        
    }
    
    template <typename Resource, typename Problem>
        bool GraphWithBounds<Resource, Problem>::testPartialDominatedAndUpdateProblemSolution(const Path<Resource> & pathTested, double & lowerBoundOnCostFromLabel) const{
       if (!isPartialDominated(pathTested, lowerBoundOnCostFromLabel)){
//#ifdef DESACTIVATE_CANDIDATE_PATHS
           addToPotentialSolutionSetIfAtDestination(pathTested);
//#else
//            completePathAndAddToPotentialSolutionSet(pathTested);
//#endif
            return false;
        } else return true;
    }



    //-----------------------------------------------------------------------------
    // Simple functor used inside function :
    //              directionMeetShortestPathAlgorithm()
    //-----------------------------------------------------------------------------
    
    template <typename Resource, typename Problem>
    CostConstraintSimpleBoundMeetAlgorithmFunctor< Resource, Problem>::CostConstraintSimpleBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> * graph0){
        graph_m = graph0;
    }
    
    template <typename Resource, typename Problem>
    bool CostConstraintSimpleBoundMeetAlgorithmFunctor< Resource, Problem>::getKey(int vertex, Resource & res) const{
        return graph_m->isDominated(vertex,res);
    }
    
    template <typename Resource, typename Problem>
    NoBoundMeetAlgorithmFunctor< Resource, Problem>::NoBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> * graph0){
        graph_m = graph0;
    }
    
    template <typename Resource, typename Problem>
    bool NoBoundMeetAlgorithmFunctor< Resource, Problem>::getKey(int vertex, Resource & res) const{
#ifdef PARTIAL_PATH_RESOURCES_TESTABLE
        return graph_m->isDominated(res);
#else
      return false;
#endif
    }
    
    //---------------------------------------------------------------------------
    // Simple functor used inside function :
    //              directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm()
    //---------------------------------------------------------------------------
    template <typename Resource, typename Problem>
    bool ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::graphCannotBeModified = false;
    
    template <typename Resource, typename Problem>
    GraphWithBounds<Resource, Problem> * ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::graph_m = 0;
    
    template <typename Resource, typename Problem>
    void ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::setGraph(GraphWithBounds<Resource, Problem> * graph0){
        if (graphCannotBeModified) {
            throw "Graph cannot be modified because functor is used elsewere";
            // If this is really needed, we only need to add inside the arguments an instance of the functor
        }
        graph_m = graph0;
    }
    
    template <typename Resource, typename Problem>
    void ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::lockGraph(){
        graphCannotBeModified = true;
    }
    
    template <typename Resource, typename Problem>
    void ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::unlockGraph(){
        graphCannotBeModified = false;
    }
    
    template <typename Resource, typename Problem>
    double ProblemOfGraphHeuristicKeyFunctor< Resource, Problem>::getKey(Path<Resource> & candidatePath){
        return graph_m->getProblem()->constraintHeuristicKey(candidatePath.getResource());
    }
//    template <typename Resource, typename Problem>
//    void DijkstraLikeAlgorithmFunctorForPathTest< Resource, Problem>::addPotentialSolutionToSolutionSet(const Path<Resource> & p)(){
//        graph_m->getProblem()->addPotentialSolutionToSolutionSet(p);
//    }
    
}
