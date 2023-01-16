//
//  ContextBuilder.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 26/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ContextBuilder.h"
//#include "GraphInContextBuilder.h"


namespace latticeRCSP {
    template<typename Resource>
    ContextBuilder<Resource>::ContextBuilder(){
       dummyOriginArcID = "";
       dummyDestinationArcID = "";
        isContextGraphBuiltAndGotFromBuilder = false;
        isDummyOriginAndDestinationAdded = false;
        isSinkVertexResourceActivated = false;
        isOneDummyArcCopyAddedForEachOriginVertex = false;
    }
    
    template<typename Resource>
    void ContextBuilder<Resource>::addSourceVertex(string const & vertexName){
        sources.insert(vertexName);
    }
    
    template<typename Resource>
    void ContextBuilder<Resource>::addSinkVertex(string const & vertexName){
        sinks.insert(vertexName);
    }
    
    template<typename Resource>
    ArcContext<Resource> * ContextBuilder<Resource>::addContextArc(NewArcContextInformation<Resource> const & newArcInformation){
        ArcContextInformation<Resource> information;
        information.originVertexName = newArcInformation.originVertexName;
        information.destinationVertexName = newArcInformation.destinationVertexName;
        information.arcContextName = newArcInformation.arcContextName;
    
        information.arcContext_m = new ArcContext<Resource>(newArcInformation.arcContextID, newArcInformation.arcContextResource);
        
        if (arcContextInformationByName.find(information.arcContextName) != arcContextInformationByName.end()) {
            throw "There is already an arc context with this name";
        }
        
        arcContextInformationByName.insert(pair<string,ArcContextInformation<Resource> >(information.arcContextName, information));
        
        return information.arcContext_m;
    }
    
   template<typename Resource>
   string ContextBuilder<Resource>::getDummyArcContextOfSinkName(string const & sinkName){
      return sinkName + "_dummySinkArc";
   }

    template<typename Resource>
    ArcContext<Resource> * ContextBuilder<Resource>::addContextArcOfSinkVertex(NewArcContextOfSinkVertexInformation<Resource> const & newArcInformation){
       ArcContextInformation<Resource> information;
       information.originVertexName = newArcInformation.sinkVertexName;
       information.destinationVertexName = "dummy_dest";


       information.arcContextName = getDummyArcContextOfSinkName(newArcInformation.sinkVertexName);

       information.arcContext_m = new ArcContext<Resource>(newArcInformation.arcContextID, newArcInformation.arcContextResource);
       if (arcContextInformationByName.find(information.arcContextName) != arcContextInformationByName.end()) {
           throw "There is already an arc context with this name";
       }
       arcContextInformationByName.insert(pair<string,ArcContextInformation<Resource> >(information.arcContextName, information));

       return information.arcContext_m;
    }

    template<typename Resource>
    ArcContext<Resource> * ContextBuilder<Resource>::getArcContextWithName(string arcName){
        
        typename map<string,ArcContextInformation<Resource> >::iterator iter = arcContextInformationByName.find(arcName);
        
        if (iter != arcContextInformationByName.end()) {
            return iter->second.arcContext_m;
        } else throw "No arc context with this name";
    }
    
    template<typename Resource>
    ContextGraph<Resource> * ContextBuilder<Resource>::getContextGraphAndRenameUnreachableContextArcs(){
        GraphInContextBuilder<Resource> builder = buildGraphOfContext();
        builder.renameArcContextsIDifTheirExtremitiesAreNotReachables();
        Graph<Resource> * solution = new Graph<Resource>(builder);
        return new ContextGraph<Resource>(getArcResourcesVector(), solution);

    }
    
    template<typename Resource>
    ContextGraph<Resource> * ContextBuilder<Resource>::getContextGraph(){
        return new ContextGraph<Resource>(getArcResourcesVector(), buildAndGetGraphOfContext());
        
    }
    
    template<typename Resource>
    vector<ArcContext<Resource> *> ContextBuilder<Resource>::getArcResourcesVector(){
        // Build the resource vector
        vector<ArcContext<Resource> *> arcResourcesVector(arcContextInformationByName.size());
        int i= 0;
        for (typename map<string, ArcContextInformation<Resource> >::iterator iter =arcContextInformationByName.begin(); iter != arcContextInformationByName.end(); ++iter) {
            arcResourcesVector[i] = iter->second.arcContext_m;
            i++;
        }
        return arcResourcesVector;
    }
    
    
    template<typename Resource>
    GraphInContextBuilder<Resource> ContextBuilder<Resource>::buildGraphOfContext(){
        GraphInContextBuilder<Resource> builder(this);
        
        NewVertexInformation infoVertex;
        NewArcInformation infoArc;
        // Build Source and sink
        if (isDummyOriginAndDestinationAdded){
             infoVertex.vertexType = SOURCE;
             infoVertex.vertexName = "dummy_ori";
             builder.addVertexIfNotPresent(infoVertex);
             infoVertex.vertexType = SINK;
              infoVertex.vertexName = "dummy_dest";
              builder.addVertexIfNotPresent(infoVertex);

             infoVertex.vertexType = INTERNAL_VERTEX;
             for (set<string>::iterator it = sources.begin(); it != sources.end(); ++it) {
                 infoVertex.vertexName = *it;
                 builder.addVertexIfNotPresent(infoVertex);
                 if (isOneDummyArcCopyAddedForEachOriginVertex){
                	 infoArc.arcContextName = getOrAddDummyArcFromOriginToVertexName(*it);
                 }
                 else {
                	 infoArc.arcContextName = getOrAddDummyArcFromOriginName();
                 }
                  infoArc.arcName = "dummyOriTo" + *it;
                  infoArc.originVertexName = "dummy_ori";
                  infoArc.destinationVertexName = *it;
                  builder.addArc(infoArc);
             }

             for (set<string>::iterator it = sinks.begin(); it != sinks.end(); ++it) {
                 infoVertex.vertexName = *it;
                 builder.addVertexIfNotPresent(infoVertex);
                 if(!isSinkVertexResourceActivated){
//                 if (isSinkVertexResourceActivated){
//                    infoArc.arcContextName = getDummyArcContextOfSinkName(*it);
//                 } else {
                    infoArc.arcContextName = getOrAddDummyArcToDestinationName();
                  infoArc.arcName = *it + "toDummyDest";
                  infoArc.originVertexName = *it;
                  infoArc.destinationVertexName = "dummy_dest";
                  builder.addArc(infoArc);
                 }
             }

        } else {
           infoVertex.vertexType = SOURCE;
           for (set<string>::iterator it = sources.begin(); it != sources.end(); ++it) {
               infoVertex.vertexName = *it;
               builder.addVertexIfNotPresent(infoVertex);
           }

           infoVertex.vertexType = SINK;
           for (set<string>::iterator it = sinks.begin(); it != sinks.end(); ++it) {
               infoVertex.vertexName = *it;
               builder.addVertexIfNotPresent(infoVertex);
           }
        }
        infoVertex.vertexType = INTERNAL_VERTEX;

        for (typename map<string, ArcContextInformation<Resource> >::iterator iter =arcContextInformationByName.begin(); iter != arcContextInformationByName.end(); ++iter) {
            if ((iter->second.arcContextName != "dummy_ori_to_vert") && (iter->second.arcContextName != "dummy_vert_to_dest")){
                infoVertex.vertexName = iter->second.originVertexName;
                builder.addVertexIfNotPresent(infoVertex);
                
                infoVertex.vertexName = iter->second.destinationVertexName;
                builder.addVertexIfNotPresent(infoVertex);
            }
        }
        
        // Build arcs
        for (typename map<string, ArcContextInformation<Resource> >::iterator iter =arcContextInformationByName.begin(); iter != arcContextInformationByName.end(); ++iter) {
            if ((iter->second.arcContextName != "dummy_ori_to_vert") && (iter->second.arcContextName != "dummy_vert_to_dest")){
                infoArc.arcContextName = iter->second.arcContextName;
                infoArc.arcName = iter->second.arcContextName;
                infoArc.originVertexName = iter->second.originVertexName;
                infoArc.destinationVertexName = iter->second.destinationVertexName;
                
                builder.addArc(infoArc);
            }
        }
        
        return builder;
    }
    
    template<typename Resource>
    Graph<Resource> * ContextBuilder<Resource>::buildAndGetGraphOfContext(){
        GraphInContextBuilder<Resource> builder = buildGraphOfContext();
        Graph<Resource> * solution = new Graph<Resource>(builder);
        
        return solution;
    }
    
    template<typename Resource>
    string ContextBuilder<Resource>::getOrAddDummyArcFromOriginName(){
        if (arcContextInformationByName.find("dummy_ori_to_vert") == arcContextInformationByName.end()) {
            NewArcContextInformation<Resource> arcInfo;
            arcInfo.originVertexName = "dummy_ori";
            arcInfo.destinationVertexName = "any_origin_vertex";
            arcInfo.arcContextName = "dummy_ori_to_vert";
            arcInfo.arcContextID = dummyOriginArcID;
            arcInfo.arcContextResource = Resource();
            addContextArc(arcInfo);
        }
        return "dummy_ori_to_vert";
    }
    
    template<typename Resource>
        string ContextBuilder<Resource>::getOrAddDummyArcFromOriginToVertexName(string const & vertexName){
    	string name = vertexName + "dummy_ori_to_vert";
    	if (arcContextInformationByName.find(name) == arcContextInformationByName.end()) {
			NewArcContextInformation<Resource> arcInfo;
			arcInfo.originVertexName = "dummy_ori";
			arcInfo.destinationVertexName = vertexName;
			arcInfo.arcContextName = name;
			arcInfo.arcContextID = vertexName + dummyOriginArcID;
			arcInfo.arcContextResource = Resource();
			addContextArc(arcInfo);
		}
    	return name;
    }

    template<typename Resource>
    string ContextBuilder<Resource>::getOrAddDummyArcToDestinationName(){
        if (arcContextInformationByName.find("dummy_vert_to_dest") == arcContextInformationByName.end()) {
            NewArcContextInformation<Resource> arcInfo;
            arcInfo.originVertexName = "any_destination_vertex";
            arcInfo.destinationVertexName = "dummy_dest";
            arcInfo.arcContextName = "dummy_vert_to_dest";
            arcInfo.arcContextID = dummyDestinationArcID;
            arcInfo.arcContextResource = Resource();
            addContextArc(arcInfo);

        }
        return "dummy_vert_to_dest";
    }
}
