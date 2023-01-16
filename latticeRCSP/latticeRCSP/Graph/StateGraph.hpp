//
//  StateGraph.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include <cassert>
#include "StateGraph.h"
#include "../Clustering/Clusterization.h"
#include "../Clustering/Cluster.h"
#include "../Clustering/ClusterFunctors.h"
#include "MeetAlgorithmFunctors/CostConstraintStateGraphBoundMeetAlgorithmFunctor.h"
#include "../Tools/Console.h"

using namespace latticeClustering;

#define RANDOM_CLUSTERS

namespace latticeRCSP {
    template <typename Resource, typename Problem>
    StateGraph<Resource, Problem>::StateGraph(bool isStateGraphReverse, GraphWithStateGraphBounds<Resource, Problem> * conditionnedGraph) : GraphWithBounds<Resource, Problem>(isStateGraphReverse, conditionnedGraph->getProblem()), isStateGraphReverse_m(isStateGraphReverse){
        
    	isStateGraphReusable_m = false;

        conditionnedGraph_m = conditionnedGraph;
        
        isStateGraphInitialized_m = false;
        
        period_s = 0.1;
        minImproveOnPeriod_percent = 0.2;
        tMax_s = 2;

        buildingResults_m = StateGraphBuildingResults();
    }
    
    //------------------------------------------------------------------
    // Getters and Setters
    //------------------------------------------------------------------
    template <typename Resource, typename Problem>
    vector<BoundAndPathList<Resource, Problem> > StateGraph<Resource, Problem>::getDirectionBoundAndPathList(){
        if (Graph<Resource>::getIsReverse() != isStateGraphReverse_m) Graph<Resource>::reverseGraph();
        
        // Set Bound And Paths if needed
//        Graph<Resource>::reverseGraph();
        if (! GraphWithBounds<Resource,Problem>::getIsBoundSet()) GraphWithBounds<Resource,Problem>::setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        
        if (! GraphWithBounds<Resource,Problem>::getIsGoodPathSet()) GraphWithBounds<Resource,Problem>::setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
//        Graph<Resource>::reverseGraph();
        
        // build the result
        vector<BoundAndPathList<Resource,Problem> > result(conditionnedGraph_m->getNVertices(),BoundAndPathList<Resource,Problem>(GraphWithBounds<Resource, Problem>::problem_m) );
        for (int v = 0; v < conditionnedGraph_m->getNVertices(); v++){
            for (int s = 0; s < stateVerticesOfVertex[v].size(); s++) {
                BoundAndPath<Resource> bp;
                bp.bound_m = Resource();
                bp.bound_m.setToSupremum();
                bp.path_m = Path<Resource>(true,-1);
                int state = stateVerticesOfVertex[v][s];
                if (isStateGraphReverse_m){
                    if (GraphWithBounds<Resource,Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state] != 0) {
                        bp.bound_m = *GraphWithBounds<Resource,Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state];
#ifndef DESACTIVATE_CANDIDATE_PATHS
                        if (GraphWithBounds<Resource,Problem>::candidatePathFromVertexToDestination[state] != 0){
                            bp.path_m = *GraphWithBounds<Resource,Problem>::candidatePathFromVertexToDestination[state];
                            convertPathFromConditionalToConditionned(bp.path_m);
                        }
#endif
                    }
                    
                } else {
                    if (GraphWithBounds<Resource,Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state] != 0) {
                        bp.bound_m = *GraphWithBounds<Resource,Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state];
#ifndef DESACTIVATE_CANDIDATE_PATHS
                        if (GraphWithBounds<Resource,Problem>::candidatePathFromOriginToVertex[state] != 0) {
                            bp.path_m = *GraphWithBounds<Resource,Problem>::candidatePathFromOriginToVertex[state];
                            convertPathFromConditionalToConditionned(bp.path_m);
                        }
#endif
                    }
                }
                result[v].insert(bp);
                
            }
        }
        
        return result;
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::setClusteringSimulatedAnnealingPeriod_s(double t){
        period_s = t;
    }
    
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::setClusteringSimulatedAnnealingMinImprovePerPeriod_percent(double val){
        minImproveOnPeriod_percent = val;
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::setClusertingSimulatedAnnealingMaxTime_s(double t){
        tMax_s = t;
    }
    //------------------------------------------------------------------
    // Redefine the direction Lower Bound Algorithm To Use The State Bound (in MeetAlgorithm of GraphWithBounds)
    //------------------------------------------------------------------
    
    template <typename Resource, typename Problem>
    vector<Resource *> StateGraph<Resource, Problem>::directionMeetShortestPathAlgorithm(){
        if (Graph<Resource>::getIsReverse() != conditionnedGraph_m->getIsReverse()) {
            auto logger = Console::getPtrLogger();
            logger->error("State graph and GraphWithStateGraphBounds are not in the same direction");
        	throw;
        }
        if (conditionnedGraph_m->getIsBoundSet()) {
            CostConstraintStateGraphBoundMeetAlgorithmFunctor<Resource, Problem> functorUsingThisGraphBounds(conditionnedGraph_m, vertexOfStateVertex);
            return GraphWithBounds<Resource, Problem>::template directionMeetShortestPathAlgorithm<CostConstraintStateGraphBoundMeetAlgorithmFunctor<Resource, Problem> > (functorUsingThisGraphBounds);
        } else {
//            cout << "noBoundFuntor" << endl;
            NoBoundMeetAlgorithmFunctor<Resource,Problem> noBoundFunctor(this);
            return GraphWithBounds<Resource, Problem>::template directionMeetShortestPathAlgorithm<NoBoundMeetAlgorithmFunctor<Resource,Problem> >(noBoundFunctor);
        }
    }
    
    
    //------------------------------------------------------------------
    // StateGraphBuilding
    //------------------------------------------------------------------
    
    template <typename Resource, typename Problem>
    template<typename ClusterizationSolver>
    void StateGraph<Resource, Problem>::buildStateGraph_acyclic(int nStatesPerVertex){
    	assert(conditionnedGraph_m->getIsAcyclic());
//    	if (!conditionnedGraph_m->getIsAcyclic()) {
//            cout << "Apply only to acyclic graph" << endl;
//            throw "Apply only to acyclic graph" ;
//        }
        if (conditionnedGraph_m->getIsReverse() != isStateGraphReverse_m) conditionnedGraph_m->reverseGraph();
        if (Graph<Resource>::getIsReverse() != isStateGraphReverse_m) Graph<Resource>::reverseGraph();
        
        initializeStateGraphStructures(nStatesPerVertex);
        pseudoTopologicalOrder_m = conditionnedGraph_m->getTopologicalOrder();
        isBuildingDirectionReverse = Graph<Resource>::getIsReverse();
        
        directionBuildStateGraph<ClusterizationSolver>(nStatesPerVertex, true);
        auto logger = Console::getPtrLogger();
        logger->debug("before setStateGraphOriginAndDestination");
        setStateGraphOriginAndDestination();
        

        isStateGraphInitialized_m = true;
        Graph<Resource>::initializeDelta();
        Graph<Resource>::initializeSourceAndSink();
        logger->debug("before Topological order");
        Graph<Resource>::initializeCyclicOrTopologicalOrder();
        if (Graph<Resource>::getIsReverse()) {
            GraphWithBounds<Resource, Problem>::isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = true;
        } else {
            GraphWithBounds<Resource, Problem>::isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = true;
        }

//        auto logger = Console::getPtrLogger();
        logger->info("State Graph built: {} vertices and {} arcs", Graph<Resource>::getNVertices(), Graph<Resource>::getNArcs());

    }
    
    template <typename Resource, typename Functor>
    double PlusEpsilonFunctor<Resource,Functor>::getKey(const Resource & r){
        return Functor::getKey(r) + EPS ;
    }
    
    template <typename Resource, typename Problem>
    template <typename VerticesOrderMorphismToRFunctor>
    void StateGraph<Resource, Problem>::buildOrderFromMorphismToR(){
        if (conditionnedGraph_m->getIsReverse() != isStateGraphReverse_m) conditionnedGraph_m->reverseGraph();
        vector <double> value = conditionnedGraph_m->template directionDijkstraAlgorithmToAllVertices< PlusEpsilonFunctor<Resource, VerticesOrderMorphismToRFunctor> >();
        multimap<double, int> sortedVertices;
        for (int i = 0; i < value.size(); i++) {
            sortedVertices.insert(pair<double,int>(value[i],i));
        }
        
//        vector<int> orderOfVertex(value.size(),-1);
        pseudoTopologicalOrder_m = vector<int>(conditionnedGraph_m->getNVertices(),-1);
        int i = 0;
        for (multimap<double, int>::iterator it = sortedVertices.begin(); it!= sortedVertices.end(); ++it){
            int j = i;
//            if (Graph<Resource>::getIsReverse()) j = conditionnedGraph_m->getNVertices() - j-1;
            pseudoTopologicalOrder_m[j] = it->second;
            i++;
            
        }
        
    }
    
    template <typename Resource, typename Problem>
    template <typename VerticesOrderMorphismToRFunctor, typename ClusterizationSolver>
    void StateGraph<Resource, Problem>::buildStateGraph_withCycle(int nStatesFirstDirection, int nStatesSecondDirection){
        assert(!conditionnedGraph_m->getIsAcyclic());
//    	if (conditionnedGraph_m->getIsAcyclic()) {
//        	cout << "Should not be used with acyclic graph" << endl;
//            throw "Should not be used with acyclic graph" ;
//        }
        if (conditionnedGraph_m->getIsReverse() != isStateGraphReverse_m) conditionnedGraph_m->reverseGraph();
        if (Graph<Resource>::getIsReverse() != isStateGraphReverse_m) Graph<Resource>::reverseGraph();
        
        clock_t t00 = clock();

        buildOrderFromMorphismToR<VerticesOrderMorphismToRFunctor>();
        
        initializeStateGraphStructures(nStatesSecondDirection + nStatesFirstDirection);
        isBuildingDirectionReverse = false;
        
        clock_t t0 = clock();
        directionBuildStateGraph<ClusterizationSolver>(nStatesFirstDirection, true);
        reverseBuildingDirection();
        clock_t t1 = clock();
        directionBuildStateGraph<ClusterizationSolver>(nStatesSecondDirection, false);
        reverseBuildingDirection();
        clock_t t2 = clock();
        directionBuildStateGraph<ClusterizationSolver>(0, false);
        clock_t t3 = clock();
        setStateGraphOriginAndDestination();
        
        Graph<Resource>::initializeDelta();
        Graph<Resource>::initializeCyclicOrTopologicalOrder();

        GraphWithBounds<Resource, Problem>::isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m = false;
        GraphWithBounds<Resource, Problem>::isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m = false;
        clock_t t4 = clock();

        buildingResults_m.nStatesFirstPhase = nStatesFirstDirection;
        buildingResults_m.nStatesSecondPhase = nStatesSecondDirection;

        buildingResults_m.nVertices = Graph<Resource>::getNVertices();
        buildingResults_m.nArcs = Graph<Resource>::getNArcs();

        buildingResults_m.timeFirstPhase = (t1 - t0) /(double) CLOCKS_PER_SEC;
        buildingResults_m.timeSecondPhase = (t2 - t1) /(double) CLOCKS_PER_SEC;
        buildingResults_m.timeThirdPhase = (t3 - t2) /(double) CLOCKS_PER_SEC;
        buildingResults_m.totalTime = (t4 - t00) /(double) CLOCKS_PER_SEC;

        auto logger = Console::getPtrLogger();
        logger->info("State Graph built: {} vertices and {} arcs", Graph<Resource>::getNVertices(), Graph<Resource>::getNArcs());
        
        
    }
    
    template <typename Resource, typename Problem>
    int StateGraph<Resource, Problem>::pseudoTopologicalOrder(int i){
        if (isBuildingDirectionReverse) {
            return pseudoTopologicalOrder_m[pseudoTopologicalOrder_m.size() - 1 - i];
        } else {
            return pseudoTopologicalOrder_m[i];
        }
    }
    
    
    template <typename Resource, typename Problem>
    template<typename ClusterizationSolver>
    void StateGraph<Resource, Problem>::directionBuildStateGraph(int nNewStatesPerVertex, bool newOriginState){
        initializeDirectionBuildingStructures();
        
    	auto logger = Console::getPtrLogger();
	   logger->info("-------------------------- Direction state graph build algorithm ------------------------");
//
//        cout << endl << "-------------------------------------" << endl << "Direction State graph building" << endl << "-------------------------------------"  << endl;
        clock_t t0 = clock();
        
//        ToolsAxel::ProgressBar bar(cout);
//        bar.init();

        logger->info("| Vert exp | Vert tot|");
        
        
        for (int v = 0; v<conditionnedGraph_m->getNVertices(); v++) {
            int currentVertex = pseudoTopologicalOrder(v);
//            cout <<  currentVertex << endl;
            if (v % 1 == 0) {
            	logger->info("|{:>9} |{:>9} |", v, conditionnedGraph_m->getNVertices());
//            	bar.update(v, conditionnedGraph_m->getNVertices());
            }
            
            if (directionOrigin.find(currentVertex) != directionOrigin.end()) {
                if (newOriginState) {
                    // Is it an origin
                    NewStateInformation<Resource> newState;
                    newState.stateResource = Resource();
                    newState.elementsInClusterInformation =  vector<IncomingStateArcInformation<Resource> >(1);
                    newState.elementsInClusterInformation[0].originState = -1;
                    newState.elementsInClusterInformation[0].networkArc = -1;
                    newState.elementsInClusterInformation[0].resource_m = Resource();
                    
                    
                    int state = buildStateInList(currentVertex,newState);
                    directionOriginStates.insert(state);
                    expandState(currentVertex, state, Resource());
                }
            } else {
                //It is not an origin
                vector<NewStateInformation<Resource> > existingStateNewElements;
                vector<NewStateInformation<Resource> > newStatesElements;
                
                logger->debug("Before Clustering");

                clusterizeIncomingResources<ClusterizationSolver>(nNewStatesPerVertex, currentVertex, existingStateNewElements, newStatesElements);
                
                
                // Expand only if it is not a destination
                logger->debug("Update state");
                for (int i = 0; i < stateVerticesOfVertex[currentVertex].size(); i++) {
                    updateState(stateVerticesOfVertex[currentVertex][i], existingStateNewElements[i]);
                }
                
                logger->debug("Expand states of vertex {} with deltaPa size {}", currentVertex, conditionnedGraph_m->deltaPa(currentVertex).size());
                for (int i = 0; i < newStatesElements.size(); i++){
                    int currentState = buildStateInList(currentVertex, newStatesElements[i]);
                    if (directionDestination.find(currentVertex) == directionDestination.end()) {
                        expandState(currentVertex, currentState, newStatesElements[i].stateResource);
                    }
                }
                logger->debug("End expand state");
            }
        }
        logger->debug("Start rebuild");
        
        rebuildDirectionStateVerticesAndArcsFromList();
        logger->debug("end rebuild");
        
        logger->debug("Start clean");
        cleanDirectionBuildingStructures_exceptVertexIncomingResources();
        logger->debug("end clean");
        
    }
    
    template <typename Resource, typename Problem>
    template<typename ClusterizationSolver>
    void StateGraph<Resource, Problem>::clusterizeIncomingResources(int nNewStatePerVertex, int vertex, vector<NewStateInformation<Resource> > & existingStatesUpdateInformation, vector<NewStateInformation<Resource> > &newStates){
        
        auto logger = Console::getPtrLogger();
        logger->debug("Start Clustering function");

        if (vertexIncomingResources[vertex].size() > nNewStatePerVertex){
            //Clusterization is needed
#ifdef RANDOM_CLUSTERS
        	// Random clusters
        	int nIncomingResources = vertexIncomingResources[vertex].size();
        	int nExistingStates = stateVerticesOfVertex[vertex].size();
        	int nStatesTotal = nNewStatePerVertex + nExistingStates;
        	Resource supremum = Resource();
        	supremum.setToSupremum();
        	unsigned long int nNewElementsPerCluster = ceil(3 + 1.1 *(double) nIncomingResources /(double) nStatesTotal);
        	vector<vector<int> > existingClustersNewElements(nExistingStates);
        	for (auto && e : existingClustersNewElements){
        		e.reserve(nNewElementsPerCluster);
        	}
			vector<Resource> existingClustersNewMeet(nExistingStates, supremum);
			for (int var = 0; var < nExistingStates; ++var) {
				int state = stateVerticesOfVertex[vertex][var];
				for (auto && r : stateClusterResources[state]){
					existingClustersNewMeet[var].meet(r);
				}
			}

			vector<vector<int> > newClustersElements(nNewStatePerVertex);
			for (auto && e : newClustersElements){
				e.reserve(nNewElementsPerCluster);
			}
			vector<Resource>  newClustersResource(nNewStatePerVertex,supremum);

			for (int incomingRes = 0; incomingRes < nIncomingResources; ++incomingRes) {
				int cl = rand() % nStatesTotal;
				if (cl < nExistingStates){
					existingClustersNewElements[cl].push_back(incomingRes);
					existingClustersNewMeet[cl].meet(vertexIncomingResources[vertex][incomingRes].resource_m);
				} else {
					newClustersElements[cl].push_back(incomingRes);
					newClustersResource[cl].meet(vertexIncomingResources[vertex][incomingRes].resource_m);
				}
			}

#else
            // Build arguments for clusterization
            vector<vector<Resource const *> > existingStates(stateVerticesOfVertex[vertex].size());
            for (int s = 0; s < stateVerticesOfVertex[vertex].size(); s++) {
                int state = stateVerticesOfVertex[vertex][s];
                existingStates[s] = vector<Resource const *>(stateClusterResources[state].size(),(Resource const *)(0));
                for (int cl = 0; cl < stateClusterResources[state].size(); cl++) {
                    existingStates[s][cl] = & stateClusterResources[state][cl];
                }
            }
            
            logger->debug("Start Building New Elements Vector");
            vector<Resource const *> newElements(vertexIncomingResources[vertex].size(), (Resource const *)(0));
            logger->debug("Start Building New Elements Iterator");
            vector<typename vector<IncomingStateArcInformation<Resource> >::iterator> incomingStateInformationVector(vertexIncomingResources[vertex].size(), vertexIncomingResources[vertex].begin());
            size_t e = 0;
            logger->debug("Start Filling New Elements Vectors Iterator");
            for (auto it = vertexIncomingResources[vertex].begin(); it != vertexIncomingResources[vertex].end(); ++it) {
                newElements[e] = &it->resource_m;
                incomingStateInformationVector[e] = it;
                e++;
            }

            
            // Clusterize
//            Clusterization<Resource, Cluster<Resource, SumOfDistancesToMeetFunctor<Resource> > > solver = Clusterization<Resource, Cluster<Resource, SumOfDistancesToMeetFunctor<Resource> > >(nNewStatePerVertex, existingStates, newElements);
            ClusterizationSolver solver = ClusterizationSolver(nNewStatePerVertex, existingStates, newElements);
            
            logger->debug("start simulated annealing");
            solver.simulatedAnnealing(period_s,minImproveOnPeriod_percent,tMax_s);
            logger->debug("End simulated annealing");
            
            // Store solution
            vector<vector<int> > existingClustersNewElements =  solver.getFixedClustersNewElements() ;
            vector<Resource> existingClustersNewMeet = solver.getFixedClustersMeet();
            vector<vector<int> > newClustersElements =  solver.getNewClustersElements() ;
            vector<Resource>  newClustersResource = solver.getNewClustersMeet();
#endif
            existingStatesUpdateInformation = vector<NewStateInformation<Resource> >(stateVerticesOfVertex[vertex].size());
            for (int s = 0; s < stateVerticesOfVertex[vertex].size(); s++) {
                NewStateInformation<Resource> info;
                info.stateResource = existingClustersNewMeet[s];
                info.elementsInClusterInformation = vector<IncomingStateArcInformation<Resource> >(existingClustersNewElements[s].size());
                for (int e = 0; e < existingClustersNewElements[s].size(); e++){
#ifdef RANDOM_CLUSTERS
                info.elementsInClusterInformation[e] = vertexIncomingResources[vertex][existingClustersNewElements[s][e]];
#else
                    info.elementsInClusterInformation[e] = *incomingStateInformationVector[existingClustersNewElements[s][e]];
#endif
                }
                existingStatesUpdateInformation[s] = info;
            }
            
            newStates = vector<NewStateInformation<Resource> >(newClustersElements.size());
            for (int s = 0; s < newClustersElements.size(); s++) {
                NewStateInformation<Resource> info;
                info.stateResource = newClustersResource[s];
                info.elementsInClusterInformation = vector<IncomingStateArcInformation<Resource> >(newClustersElements[s].size());
                for (int e = 0; e < newClustersElements[s].size(); e++){
#ifdef RANDOM_CLUSTERS
                    info.elementsInClusterInformation[e] = vertexIncomingResources[vertex][newClustersElements[s][e]];
#else
                    info.elementsInClusterInformation[e] = *incomingStateInformationVector[newClustersElements[s][e]];
#endif
                }
                newStates[s] = info;
            }

        }
        else {
            // Clusterization is not needed
            NewStateInformation<Resource> noUpdate;
            noUpdate.stateResource = Resource();
            noUpdate.elementsInClusterInformation = vector<IncomingStateArcInformation<Resource> >(0);
            existingStatesUpdateInformation = vector<NewStateInformation<Resource> >(stateVerticesOfVertex[vertex].size(), noUpdate);
            
            newStates = vector<NewStateInformation<Resource> >(vertexIncomingResources[vertex].size());
            size_t e = 0;
            for (auto it = vertexIncomingResources[vertex].begin(); it != vertexIncomingResources[vertex].end(); ++it) {
                NewStateInformation<Resource> info;
                
                info.stateResource = it->resource_m;
                info.elementsInClusterInformation = vector<IncomingStateArcInformation<Resource> >(1);
                info.elementsInClusterInformation[0] = *it;
                
                newStates[e] = info;
                e++;
            }
            
        }
        vertexIncomingResources[vertex].clear();
        logger->debug("End clustering function");
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::cleanDirectionBuildingStructures_exceptVertexIncomingResources(){
//        vertexIncomingResources = vector<list<IncomingStateArcInformation<Resource> > >(0);
        stateResource.clear();
        stateClusterResourcesList.clear();
        stateArcsOfNetworkArcList.clear();
        stateArcsOfNetworkArcList.shrink_to_fit();
        stateVerticesOfNetworkVertexList.clear();
        stateVerticesOfNetworkVertexList.shrink_to_fit();
        
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::rebuildDirectionStateVerticesAndArcsFromList(){
        //-----------------
        // Rebuild the arcs
        //-----------------
        auto logger = Console::getPtrLogger();
    	logger->debug("Before Vector of arcs built");
    	
        vector<Arc<Resource> > newArcs(stateArcsList.size());
        logger->debug("Vector of arcs built");
        

        vector<int> newNetworkArcOfStateArc(stateArcsList.size());
        // Rebuilds arcs
        int a = 0;
        for (list<StateArcInformation>::iterator it = stateArcsList.begin(); it != stateArcsList.end(); ++it){
            newArcs[a] = Arc<Resource>(conditionnedGraph_m->getArc(it->networkArc).getArcContext() , it->origin, it->destination, Graph<Resource>::getIsReverse());
            newNetworkArcOfStateArc[a] = it->networkArc;
            a++;
        }
        Graph<Resource>::Arcs.insert(Graph<Resource>::Arcs.end(), newArcs.begin(), newArcs.end());
        arcOfStateArc.insert(arcOfStateArc.end(), newNetworkArcOfStateArc.begin(), newNetworkArcOfStateArc.end());
        
        for (a = 0; a < stateArcsOfArc.size(); a++){
            vector<int> newStates(stateArcsOfNetworkArcList[a].size());
            int s = 0;
            for (list<int>::iterator it = stateArcsOfNetworkArcList[a].begin(); it != stateArcsOfNetworkArcList[a].end(); ++it) {
                newStates[s] = *it;
                ++s;
            }
            stateArcsOfArc[a].insert(stateArcsOfArc[a].end(), newStates.begin(), newStates.end());
        }
        
        logger->debug("Vector of arcs filled");
        

        //-----------------
        // Rebuild the vertices
        //-----------------
        vertexOfStateVertex.resize(Graph<Resource>::nVertices, -1);
        for (int v = 0; v < stateVerticesOfNetworkVertexList.size(); v++){
            vector<int> newStates(stateVerticesOfNetworkVertexList[v].size());
            int s = 0;
            
            for (list<int>::iterator it = stateVerticesOfNetworkVertexList[v].begin(); it != stateVerticesOfNetworkVertexList[v].end(); ++it) {
                newStates[s] = *it;
                vertexOfStateVertex[*it] = v;
                s++;
            }
            stateVerticesOfVertex[v].insert(stateVerticesOfVertex[v].end(), newStates.begin(), newStates.end());
            
        }
        
        unsigned int firstNewState = stateClusterResources.size();
        unsigned int state = firstNewState;
        stateClusterResources.resize(Graph<Resource>::nVertices);
        for (typename list<vector<Resource> >::iterator stateClusterIter = stateClusterResourcesList.begin(); stateClusterIter != stateClusterResourcesList.end(); ++stateClusterIter){
            stateClusterResources[state].insert(stateClusterResources[state].end(), stateClusterIter->begin(), stateClusterIter->end());
            state++;
        }
        
        state = firstNewState;
        if (isStateGraphReverse_m){
            GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination.resize(Graph<Resource>::getNVertices(), (Resource *)0);
            for (typename list<Resource>::iterator it = stateResource.begin(); it != stateResource.end(); ++it){
                GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state] = new Resource(*it);
                state++;
            }
        } else {
            GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex.resize(Graph<Resource>::getNVertices(), (Resource *)0);
            for (typename list<Resource>::iterator it = stateResource.begin(); it != stateResource.end(); ++it){
                GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state] = new Resource(*it);
                state++;
            }
        }
        
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::updateState(int state, const NewStateInformation<Resource> & info){
        // Build new arcs
        if (info.elementsInClusterInformation.size() > 0) {
            for (typename vector<IncomingStateArcInformation<Resource> >::const_iterator it = info.elementsInClusterInformation.cbegin(); it != info.elementsInClusterInformation.end(); ++it) {
                StateArcInformation info;
                info.origin = it->originState;
                info.destination = state;
                info.networkArc = it->networkArc;
                stateArcsList.push_back(info);
                stateArcsOfNetworkArcList[it->networkArc].push_back(Graph<Resource>::nArcs);
                Graph<Resource>::nArcs++;
            }
            
            // Build new state
            if (isStateGraphReverse_m){
                if (GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state] != 0) delete GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state];
                GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state] = new Resource(info.stateResource);
            } else {
                if (GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state] != 0) delete GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state];
                GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state] = new Resource(info.stateResource);
            }
            
            vector<Resource> clusterResources = vector<Resource>(info.elementsInClusterInformation.size());
            for (int r = 0; r < info.elementsInClusterInformation.size(); r++) {
                clusterResources[r] = info.elementsInClusterInformation[r].resource_m;
            }
            stateClusterResources[state].insert(stateClusterResources[state].end(), clusterResources.begin(),clusterResources.end());
        }
    }
    
    
    template <typename Resource, typename Problem>
    int StateGraph<Resource, Problem>::buildStateInList(int networkVertex, NewStateInformation<Resource> const & info){
        // Build new arcs
        for (typename vector<IncomingStateArcInformation<Resource> >::const_iterator it = info.elementsInClusterInformation.cbegin(); it != info.elementsInClusterInformation.end(); ++it) {
            if (it->originState != -1) { // If it is not an origin
                StateArcInformation info;
                info.origin = it->originState;
                info.destination = Graph<Resource>::nVertices;
                info.networkArc = it->networkArc;
                stateArcsList.push_back(info);
                stateArcsOfNetworkArcList[it->networkArc].push_back(Graph<Resource>::nArcs);
                Graph<Resource>::nArcs++;
            }
        }
        
        // Build new state
        stateVerticesOfNetworkVertexList[networkVertex].push_back(Graph<Resource>::nVertices);
        stateResource.push_back(info.stateResource);
        vector<Resource> clusterResources(info.elementsInClusterInformation.size());
        for (int r = 0; r < info.elementsInClusterInformation.size(); r++) {
            clusterResources[r] = info.elementsInClusterInformation[r].resource_m;
        }
        stateClusterResourcesList.push_back(clusterResources);
        
        int state = Graph<Resource>::nVertices;
        Graph<Resource>::nVertices++;
        return state;
    }
    
    template <typename Resource, typename Problem>
    bool StateGraph<Resource, Problem>::isResourceDominatedByOneStateOfTheVertex(int vertex, const Resource &testedResource){
       auto logger = Console::getPtrLogger();
       logger->debug("State dominance tested");
        for (int i = 0; i < stateVerticesOfVertex[vertex].size(); i++) {
            Resource stateResource;
            int state = stateVerticesOfVertex[vertex][i];
            if (isStateGraphReverse_m){
                stateResource = *GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination[state];
            } else {
                stateResource = *GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex[state];
                
            }
            if (testedResource.isGeq(stateResource)) {
                return true;
            }
        }
        
        for (auto iter = vertexIncomingResources[vertex].begin(); iter != vertexIncomingResources[vertex].end(); ++iter ){
            if (testedResource.isGeq(iter->resource_m)){
                return true;
            }
        }
        
        return false;
    }
    
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::expandState(int vertex, int state, const Resource &stateResource){
        vector<int> const & delta = conditionnedGraph_m->deltaPa(vertex);
        auto logger = Console::getPtrLogger();
        for (int a = 0; a < delta.size(); a++) {
            int arc = delta[a];
            int destinationVertex = conditionnedGraph_m->arcDestination(arc);
            
            Resource newResource = Resource();
            if (Graph<Resource>::getIsReverse()){
            	newResource.expand(conditionnedGraph_m->getArc(arc).getResource());
            	newResource.expand(stateResource);
            } else {
                newResource = stateResource;
            	newResource.expand(conditionnedGraph_m->getArc(arc).getResource());
            }
            
//            if (true){ //column generation cout << "warning this must be desactivated and next line activated" << endl;
            if (isStateGraphReusable_m || ! isResourceDominatedByOneStateOfTheVertex(destinationVertex, newResource)){
                IncomingStateArcInformation<Resource> newArcInformation;
                newArcInformation.originState = state;
                newArcInformation.networkArc = arc;
                newArcInformation.resource_m = newResource;
                
                vertexIncomingResources[destinationVertex].push_back(newArcInformation);
            }
        }
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::initializeStateGraphStructures(int nStatesPerVertex){
        Graph<Resource>::nVertices = 0;
        Graph<Resource>::nArcs = 0;
        stateVerticesOfVertex = vector<vector<int> >(conditionnedGraph_m->getNVertices());
        vertexIncomingResources = vector<vector<IncomingStateArcInformation<Resource> > >(conditionnedGraph_m->getNVertices());
        for (int vertex = 0; vertex < vertexIncomingResources.size(); ++vertex) {
			vertexIncomingResources[vertex].reserve(conditionnedGraph_m->deltaMa(vertex).size() *  nStatesPerVertex);
		}
        GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromVertexToDestination = vector<Resource *>(0);
        GraphWithBounds<Resource, Problem>::LowerBoundOnProblemSolutionSubPathsFromOriginToVertex = vector<Resource *>(0);
        
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::reverseBuildingDirection(){
        isBuildingDirectionReverse = !isBuildingDirectionReverse;
    }
    
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::initializeDirectionBuildingStructures(){
//        stateResource = list<Resource>();
//        stateArcsList = list<pair<int, int> >() ;
        stateArcsOfNetworkArcList = vector<list<int> >(conditionnedGraph_m->getNArcs());
        stateVerticesOfNetworkVertexList = vector<list<int> >(conditionnedGraph_m->getNVertices());
        
        if (Graph<Resource>::getIsReverse() != conditionnedGraph_m->getIsReverse()){
            directionOrigin = conditionnedGraph_m->getDestination();
            directionDestination = conditionnedGraph_m->getOrigin();
        } else {
            directionOrigin = conditionnedGraph_m->getOrigin();
            directionDestination = conditionnedGraph_m->getDestination();
        }
        
        
    }
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::setStateGraphOriginAndDestination(){
        set<int> vertexOri = conditionnedGraph_m->getOrigin_withoutDirection();
        set<int> stateOri = set<int>();
        for (set<int>::iterator iter = vertexOri.begin(); iter != vertexOri.end(); ++iter){
            int vertex = *iter;
            for (int s = 0 ; s < stateVerticesOfVertex[vertex].size(); s++) {
                stateOri.insert(stateVerticesOfVertex[vertex][s]);
            }
        }
        
        set<int> vertexDest = conditionnedGraph_m->getDestination_withoutDirection();
        set<int> stateDest = set<int>();
        for (set<int>::iterator iter = vertexDest.begin(); iter != vertexDest.end(); ++iter){
            int vertex = *iter;
            for (int s = 0 ; s < stateVerticesOfVertex[vertex].size(); s++) {
                stateDest.insert(stateVerticesOfVertex[vertex][s]);
            }
        }
        
        Graph<Resource>::setOriginDestination_withoutDirection(stateOri, stateDest);
        
    }
    
    
    template <typename Resource, typename Problem>
    void StateGraph<Resource, Problem>::convertPathFromConditionalToConditionned(Path<Resource> & path0) const{
        path0.setOrigin_neverReverse(vertexOfStateVertex[path0.getOrigin_neverReverse()]);
        path0.setDestination_neverReverse(vertexOfStateVertex[path0.getDestination_neverReverse()]);
        
    }
    
}
