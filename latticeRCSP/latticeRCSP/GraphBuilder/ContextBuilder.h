//
//  ContextBuilder.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 26/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ContextBuilder__
#define __latticeRCSP_v2__ContextBuilder__

#include <stdio.h>
#include <map>
#include <string>
#include <list>

#include "NewVertexAndArcContextStructures.h"
#include "GraphInContextBuilder.h"
#include "../Context/ArcContext.h"
#include "../Tools/ToolsForLatticeRCSP.h"


namespace latticeRCSP {
    
//    struct NewVertexContextInformation{
//        string vertexName;
//    };
//    
//    struct VertextContextInformation{
//        string vertexName;
//        int vertexIndexInContextGraph;
//    };
    
    template<typename Resource>
    struct NewArcContextInformation{
        string originVertexName;
        string destinationVertexName;
        string arcContextName; // Local to ContextBuilder
        
        string arcContextID; // Id in ArcContext
        Resource arcContextResource;
    };
    
    template<typename Resource>
    struct NewArcContextOfSinkVertexInformation{
        string sinkVertexName;

        string arcContextID; // Id in ArcContext
        Resource arcContextResource;
    };

    template<typename Resource>
    struct ArcContextInformation{
        string originVertexName;
        string destinationVertexName;
        string arcContextName;
        ArcContext<Resource> * arcContext_m;
    };
    
    template<typename Resource>
    class GraphInContextBuilder;
    
    template<typename Resource>
    class ContextGraph;
    
    template<typename Resource>
    class Graph;
    
    struct NewVertexInformation;
    struct NewArcInformation;
    
    template <typename Resource>
    class ContextBuilder {
//         This class enables to build a graphContext by adding arc identified by their name
//            - it does not destroy the memory it allocates : this memory must be destroyed from the arcContext itserf
        
        string dummyOriginArcID;
        string dummyDestinationArcID;

        set<string> sources;
        set<string> sinks;
//        map<string, Resource> sinks;
        map<string, ArcContextInformation<Resource> > arcContextInformationByName;
        
        bool isDummyOriginAndDestinationAdded;
        bool isSinkVertexResourceActivated;
        bool isContextGraphBuiltAndGotFromBuilder;
        bool isOneDummyArcCopyAddedForEachOriginVertex;
        
    public:
        ContextBuilder();
        
        void setDummyOriginArcID(string const & id) {dummyOriginArcID = id;}
        void setDummyDestinationArcID(string const & id) {dummyDestinationArcID = id;}
        void setIsDummyOriginAndDestinationAdded(bool const & is){ isDummyOriginAndDestinationAdded = is;}
        void setIsSinkVertexResourceActivated(bool is){ isSinkVertexResourceActivated = is;}
        void setIsOneDummyArcCopyAddedForEachOriginVertex(bool is){isOneDummyArcCopyAddedForEachOriginVertex = is;}

//        void addContextVertex(NewVertexContextInformation);
        void addSourceVertex(string const &);
        void addSinkVertex(string const &);
        ArcContext<Resource> * addContextArc(NewArcContextInformation<Resource> const &);
        ArcContext<Resource> * addContextArcOfSinkVertex(NewArcContextOfSinkVertexInformation<Resource> const &);
        ArcContext<Resource> * getArcContextWithName(string);
        list<ArcContext<Resource> * > getOutgoingArcContextOfVertexName(string);
        
        ContextGraph<Resource> * getContextGraphAndRenameUnreachableContextArcs();
        ContextGraph<Resource> * getContextGraph();
    private:
        GraphInContextBuilder<Resource> buildGraphOfContext();
        Graph<Resource> * buildAndGetGraphOfContext();
        vector<ArcContext<Resource> *> getArcResourcesVector();
        string getOrAddDummyArcFromOriginName();
        string getOrAddDummyArcFromOriginToVertexName(string const &);
        string getOrAddDummyArcToDestinationName();
        static string getDummyArcContextOfSinkName(string const &);
    };
}



#include "ContextBuilder.hpp"

#endif /* defined(__latticeRCSP_v2__ContextBuilder__) */
