//
//  GraphInContextBuilder.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 26/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__GraphInContextBuilder__
#define __latticeRCSP_v2__GraphInContextBuilder__

#include <stdio.h>

#include "NewVertexAndArcContextStructures.h"
#include "ContextBuilder.h"
#include "../Graph/Graph.h"
#include "../Context/ArcContext.h"
#include "../Context/ContextGraph.h"

namespace latticeRCSP {    
    template<typename Resource>
    class ContextBuilder;
    
    template<typename Resource>
    class Graph;
    
    template<typename Resource>
    class GraphInContextBuilder{
        ContextBuilder<Resource> * context_m;
        map<string, VertexInformation> vertices;
        map<string, ArcInformation<Resource> > arcs;
        
    public:
        GraphInContextBuilder(ContextBuilder<Resource> *);
        
        void addVertexIfNotPresent(NewVertexInformation);
        void addArcAndThrowErrorIfItsVerticesAreNotPresent(NewArcInformation);
        void addArc(NewArcInformation);
        
        
        
        void addOriginAndDestinationVertices();
        void removeVerticesUnreachableFromOriginOrDestination();
        
        void renameArcContextsIDifTheirExtremitiesAreNotReachables();
        
        map<string, VertexInformation> getVertices() const;
        map<string, ArcInformation<Resource> > getArcs() const;
        
        static string getArcContextIDarcWithOriginUnreachable();
        static string getArcContextIDarcWithDestinationUnreachable();
        static string getArcContextIDarcWithOriginAndDestinationUnreachable();
                
    protected:
        void updateVertexReachabilityFromOriginToTrue(string);
        void updateVertexReachabilityFromDestinationToTrue(string);
        void deleteVertex(string);
        void deleteArc(string);
        
    public:
        static string arcContextIDarcWithOriginUnreachable;
        static string arcContextIDarcWithDestinationUnreachable;
        static string arcContextIDarcWithOriginAndDestinationUnreachable;
                
    };
}

#include "GraphInContextBuilder.hpp"

#endif /* defined(__latticeRCSP_v2__GraphInContextBuilder__) */
