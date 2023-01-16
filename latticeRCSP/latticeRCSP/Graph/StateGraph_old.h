//
//  stateGraph.h
//  RCSP
//
//  Created by Axel Parmentier on 14/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__stateGraph__
#define __RCSP__stateGraph__

#include <iostream>
#include "latticeClusterization.h"
#include "BandPSetList.h"

//#include "kSTcluster.h"

namespace latticeRCSP {
    template<typename resource, typename networkGraphType>
    class StateGraph : public resourceGraph<resource> {
    protected:
        
        
        bool isStateGraphReverse;
        
        vector<vector<int> > stateVerticesOfVertex;
        vector<int> vertexOfState;
        vector<vector<int> > stateArcsOfArc;
        vector<int> arcOfStateArc;
        titap.pdf
    public:
        StateGraph(bool isReverse);
        
        void buildStateGraph(int, double,double,double);
        void expandResourceAlongArc(int vertex, int state, const resource & r, vector<list<pair<pair<int, int>, resource> > > & vertexIncomingResources);
        
        template<typename goodPathSetKeyFunctor>
        vector<BandPSetList<boundAndPaths<resource> > > getBoundSet() ;
        template <typename argumentBoundSet>
        vector<BandPSetList<argumentBoundSet> > getBoundSet(vector<argumentBoundSet> &) ;
        
        int getNetworkArcRecursive(int arc) const; // While the networkGraph is a stateGraph / a conditionalGraph, goes recursively to find the networkGraph (which must be a boundGraph<resource>
        void setRecursiveArcID();
        
    };
}

#include "stateGraph.hpp"


#endif /* defined(__RCSP__stateGraph__) */
