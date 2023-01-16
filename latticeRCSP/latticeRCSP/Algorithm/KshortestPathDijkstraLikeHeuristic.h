//
//  KshortestPathDijkstraLikeHeuristic.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 16/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__KshortestPathDijkstraLikeHeuristic__
#define __latticeRCSP_v2__KshortestPathDijkstraLikeHeuristic__

#include <stdio.h>
#include <vector>
#include <map>

#include "../Graph/Path.h"




namespace latticeRCSP {
    template <typename Resource, typename HeuristicMinimizedFunctor>
    class KshortestPathDijkstraLikeHeuristic {
        Graph<Resource> * graph_m;
        int nPathsSearched;
        vector<unsigned int> nPathsToVertexFound;
        multimap<double, Path<Resource> > pathToBeExpandedMap;
        
        bool acyclicPathCheck;
        
        PathSet<Resource, HeuristicMinimizedFunctor> solution;
        
    public:
        KshortestPathDijkstraLikeHeuristic(Graph<Resource> *, int);
        ~KshortestPathDijkstraLikeHeuristic();
        
        void solve();
        PathSet<Resource, HeuristicMinimizedFunctor>  getSolution();
        
    private:
        
    };
}

#include "KshortestPathDijkstraLikeHeuristic.hpp"

#endif /* defined(__latticeRCSP_v2__KshortestPathDijkstraLikeHeuristic__) */
