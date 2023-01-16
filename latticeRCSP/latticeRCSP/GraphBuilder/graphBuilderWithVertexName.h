//
//  graphBuilderWithVertexName.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 30/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__graphBuilderWithVertexName__
#define __latticeRCSP__graphBuilderWithVertexName__

#include <stdio.h>
#include <list>
#include <map>




//!!! add the qualifier isBase to vertices
//!!! add one origin and one destination vertex and arcs between the origin vertex and all the vertices
//!!! add the possibility to remove unreachable vertices (this way the origin and destination we obtain are really the origin and destination we want)


using namespace std;

namespace latticeRCSP{
    
    struct vertexInformation{
        string name;
        int type; // -1 : origin, 0 : internal, 1 : destination
        int index;
        list<int> deltaP;
        list<int> deltaM;
        
        int isReachable; // 0 if not reachable, 1 if only from source, 2 if only from destination, 3 if from both
    };
    
    struct arcInformation{
        bool isInterDuty;
        
        string originName;
        string destinationName;
    };
    
    template <typename resource>
    class graphBuilderWithVertexName {
        int nVertices;
        int nArcs;
        
        bool isOriginDestination;
        string originVertex;
        string destinationVertex;
        
        // In the graph builders, there are resource both on the arcs and on the vertices. When the graph is returned, the arc resources are obtained by summing there incoming vertex resource and the arc resource
        map<string,pair<vertexInformation,resource> > vertices;
        map<int,pair<arcInformation,resource> > Arcs;
        
        
    public:
        graphBuilderWithVertexName();
        
        //int getOrCreateVertex(string);
        
        int insertVertex(resource &, int type);
        int insertArc(string,string, resource &, bool isInterDuty);// Both string corresponds to the arc
        //int insertArc(int,int,resource);
        
        void initializeDelta();
        
        void deleteVertex(string vertex);
        void addOriginAndDestination();
        void removeUnreachableVerticesAndArcs();
        void reIndexVerticesAndArcs();
        
        int getNVertices();
        vector<pair<int, int> > getArcs();
        vector<resource> getArcResources();
        
        
        
        
    };
}
#include "graphBuilderWithVertexName.hpp"

#endif /* defined(__latticeRCSP__graphBuilderWithVertexName__) */
