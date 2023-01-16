//
//  GraphBuilder.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 08/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "GraphBuilder.h"

namespace latticeRCSP{
    template <typename resource>
    GraphBuilder<resource>::GraphBuilder(){
        nVertices = 0;
        nArcs = 0;
    }
    
    template <typename resource>
    GraphBuilder<resource>::GraphBuilder(list<pair<int,int> >lA,list<resource> lR, bool isODSet, int ori, int dest){
        if (lA.size() != lR.size()) throw "Arcs ans arc resourcees list must have the same size";
        Arcs = lA;
        arcResources = lR;
        nArcs = lA.size();
        nVertices = 0;
        for (list<pair<int, int > >::iterator it = Arcs.begin(); it!=Arcs.end(); ++it) {
            nVertices = max(nVertices,it->first + 1);
            nVertices = max(nVertices,it->second + 1);
        }
        
        isOriginDestinationSet = isODSet;
        if (isODSet) {
            origin = ori;
            destination = dest;
            
        }
    }
    
    template <typename resource>
    int GraphBuilder<resource>::insertVertex(){
        nVertices++;
        return nVertices -1;
    }
    
    template <typename resource>
    int GraphBuilder<resource>::insertArc(int o, int d, resource r){
        nArcs++;
        Arcs.push_back(pair<int,int>(o,d));
        arcResources.push_back(r);
        return nArcs -1;
    }
    
    template <typename resource>
    int GraphBuilder<resource>::getNVertices() {
        return nVertices;
    }
    
    template <typename resource>
    list<pair<int,int> > const & GraphBuilder<resource>::getArcs() const{
        return Arcs;
    }
    
    template <typename resource>
    list<resource> const & GraphBuilder<resource>::getArcResources() const{
        return arcResources;
    }
    
    template <typename resource>
    int GraphBuilder<resource>::getDestination() const {
        return destination;
    }
    
    template <typename resource>
    int GraphBuilder<resource>::getOrigin() const{
        return origin;
    }
    
    template <typename resource>
    bool GraphBuilder<resource>::getIsOriginDestinationSet() const{
        return isOriginDestinationSet;
    }
}

