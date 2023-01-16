//
//  NewVertexAndArcContextStructures.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 30/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef latticeRCSP_v2_NewVertexAndArcContextStructures_h
#define latticeRCSP_v2_NewVertexAndArcContextStructures_h

#include <string>
#include "../Context/ArcContext.h"

using namespace std;

namespace latticeRCSP {
    
    typedef enum {
        SOURCE,
        SINK,
        INTERNAL_VERTEX
    } VertexTypes;
    
    struct NewVertexInformation{
        string vertexName;
        VertexTypes vertexType; // +1 origin, -1 destination, 0 internal vertex
    };
    
    struct NewArcInformation{
        string arcContextName;
        string arcName;
        
        string originVertexName;
        string destinationVertexName;
    };
    
    
    struct VertexInformation {
        string vertexName;
        VertexTypes vertexType;
        
        list<string> outgoingArcs;
        list<string> incomingArcs;
        
        bool isReachableFromOrigin;
        bool isReachableFromDestination;
    };
    
    template<typename Resource>
    struct ArcInformation{
        ArcContext<Resource> * arcContext_m;
        string arcName;
        
        string originVertexName;
        string destinationVertexName;
    };
}


#endif
