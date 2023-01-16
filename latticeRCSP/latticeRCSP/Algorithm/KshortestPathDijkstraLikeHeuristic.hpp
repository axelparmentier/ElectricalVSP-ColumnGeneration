//
//  KshortestPathDijkstraLikeHeuristic.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 16/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "KshortestPathDijkstraLikeHeuristic.h"

namespace latticeRCSP {
    template <typename Resource, typename HeuristicMinimizedFunctor>
    KshortestPathDijkstraLikeHeuristic<Resource,HeuristicMinimizedFunctor>::KshortestPathDijkstraLikeHeuristic(Graph<Resource> * graph0, int k){
        graph_m = graph0;
        nPathsSearched = k;
        acyclicPathCheck = true;
        
    }
    
    template <typename Resource, typename HeuristicMinimizedFunctor>
    KshortestPathDijkstraLikeHeuristic<Resource,HeuristicMinimizedFunctor>::~KshortestPathDijkstraLikeHeuristic(){
        cout << "Appel du destructeur" << endl;
        
    }
    
    template <typename Resource, typename HeuristicMinimizedFunctor>
    void KshortestPathDijkstraLikeHeuristic<Resource,HeuristicMinimizedFunctor>::solve(){
        pathToBeExpandedMap.clear();
        
        solution.clear();
        solution.setMaxSize(nPathsSearched);

        int nVertices = graph_m->getNVertices();
        nPathsToVertexFound = vector<unsigned int>(nVertices,0);
        
//        vector<int> vertexType(nVertices);
//        for (int v = 0; v<nVertices; v++) {
//            vertexType[v] = graph_m->vertexType(v);
//        }
        
        // Initialization
        set<int> origin = graph_m->getOrigin();
        for (set<int>::iterator it = origin.begin(); it != origin.end(); ++it) {
            pathToBeExpandedMap.insert(pair<double,Path<Resource> >(0, Path<Resource>( graph_m->getIsReverse(), *it)));
        }
        
        while (solution.size() < nPathsSearched && !pathToBeExpandedMap.empty()) {
            typename multimap<double, Path<Resource> >::iterator it = pathToBeExpandedMap.begin();
            Path<Resource> currentPath = it->second;
            int currentVertex = currentPath.getDestination();
            pathToBeExpandedMap.erase(it);
            
            if (graph_m->vertexType(currentVertex) != -1) {
                nPathsToVertexFound[currentVertex]++;
                // expand
                for (int a = 0; a < graph_m->deltaPa(currentVertex).size(); a++) {
                    Arc<Resource> currentArc = graph_m->getArc(graph_m->deltaPa(currentVertex)[a]);
                    if (!currentPath.testIfArcBelongsToPath(currentArc) && (nPathsToVertexFound[currentArc.getDestination()] < nPathsSearched)) {
                        Path<Resource> expandedPath = currentPath;
                        expandedPath.expand(currentArc);
                        pathToBeExpandedMap.insert(pair<double, Path<Resource> >(HeuristicMinimizedFunctor::getKey(expandedPath.getResource()), expandedPath));
                    }
                }
            } else {
                // If destination add to solution set
                cout << "Destination : " << currentPath.getDestination() << " " ;
                 solution.insert(currentPath);
                cout << "INsertion" << endl;
            }
        }
        
        cout << "fin de dijkstra" << endl;
        pathToBeExpandedMap.clear();
        cout << "après clear" << endl;

//        solution = PathSet<Resource, keyFunctor>(nPathsSearched);
//        destination = graph_m->getDestination();
//        for (set<int>::iterator it = destination.begin(); it != destination.end(); ++it) {
//            for (int i = 0; i< nPathsSearched; i++){
//                solution.insert(
//            }
//        }
        
    }
    
    template <typename Resource, typename HeuristicMinimizedFunctor>
    PathSet<Resource, HeuristicMinimizedFunctor> KshortestPathDijkstraLikeHeuristic<Resource,HeuristicMinimizedFunctor>::getSolution(){
        return solution;
    }
    
}