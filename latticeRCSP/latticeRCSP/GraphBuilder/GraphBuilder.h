//
//  GraphBuilder.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 08/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__GraphBuilder__
#define __latticeRCSP__GraphBuilder__

#include <stdio.h>
#include <list>

using namespace std;

namespace latticeRCSP{
    
template <typename resource>
class GraphBuilder {
    int nVertices;
    int nArcs;
    
    bool isOriginDestinationSet;
    int origin;
    int destination;
    
    //smap<string,int> vertices;
    list<pair<int, int> > Arcs;
    list<resource> arcResources;
    
public:
    GraphBuilder();
    GraphBuilder(list<pair<int, int> >,list<resource>, bool isOriginDestinationSet = false, int origin = -1, int destination = -1);
    
    int getOrCreateVertex(string);
    
    int insertVertex();
    //int insertArc(string,string,resource);// Both string corresponds to the arc
    int insertArc(int,int,resource);
    
    int getNVertices();
    list<pair<int, int> > const & getArcs() const;
    list<resource> const & getArcResources()const;
    
    bool getIsOriginDestinationSet() const;
    int getOrigin() const;
    int getDestination() const;
    
    
};
}
#include "GraphBuilder.hpp"

#endif /* defined(__latticeRCSP__GraphBuilder__) */
