//
//  GraphInContextBuilder.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 26/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "GraphInContextBuilder.h"
#include "NewVertexAndArcContextStructures.h"

namespace latticeRCSP {
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::arcContextIDarcWithDestinationUnreachable = "___destination_unreachable___";
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::arcContextIDarcWithOriginAndDestinationUnreachable = "___origin_and_destination_unreachable___";
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::arcContextIDarcWithOriginUnreachable = "___origin_unreachable___";
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::getArcContextIDarcWithOriginUnreachable(){
        return arcContextIDarcWithOriginUnreachable;
    }
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::getArcContextIDarcWithOriginAndDestinationUnreachable(){
        return arcContextIDarcWithOriginAndDestinationUnreachable;
    }
    
    template<typename Resource>
    string GraphInContextBuilder<Resource>::getArcContextIDarcWithDestinationUnreachable(){
        return arcContextIDarcWithDestinationUnreachable;
    }
    
    template<typename Resource>
    GraphInContextBuilder<Resource>::GraphInContextBuilder(ContextBuilder<Resource> * context){
        context_m = context;
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::addVertexIfNotPresent(latticeRCSP::NewVertexInformation vertexInfo){
        if (vertices.find(vertexInfo.vertexName) == vertices.end()) {
            VertexInformation newVertexInfo;
            newVertexInfo.vertexName = vertexInfo.vertexName;
            newVertexInfo.vertexType = vertexInfo.vertexType;
            
            if (newVertexInfo.vertexType == SOURCE) {
                newVertexInfo.isReachableFromOrigin = true;
                newVertexInfo.isReachableFromDestination = false;
            } else if (newVertexInfo.vertexType == SINK){
                newVertexInfo.isReachableFromOrigin = false;
                newVertexInfo.isReachableFromDestination = true;
            } else {
                newVertexInfo.isReachableFromOrigin = false;
                newVertexInfo.isReachableFromDestination = false;
            }
            
            vertices.insert(pair<string, VertexInformation>(newVertexInfo.vertexName, newVertexInfo));
        }
    }
    
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::addArcAndThrowErrorIfItsVerticesAreNotPresent(latticeRCSP::NewArcInformation arcInfo){
        if ((vertices.find(arcInfo.originVertexName) == vertices.end()) || (vertices.find(arcInfo.destinationVertexName) == vertices.end())) {
            cout << "Arc extremities are not present" << endl;
            throw "Arc extremities are not present";
        }
        
        addArc(arcInfo);
        
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::addArc(latticeRCSP::NewArcInformation arcInfo){

        if (arcs.find(arcInfo.arcName) == arcs.end()) {
            // Check origin and destination vertex presence
            NewVertexInformation ori;
            ori.vertexType = INTERNAL_VERTEX;
            ori.vertexName = arcInfo.originVertexName;
            addVertexIfNotPresent(ori);
            ori.vertexName = arcInfo.destinationVertexName;
            addVertexIfNotPresent(ori);
            
            // addArc
            ArcInformation<Resource> newArcInfo;
            newArcInfo.arcContext_m = context_m->getArcContextWithName(arcInfo.arcContextName);
            newArcInfo.arcName = arcInfo.arcName;
            
            newArcInfo.originVertexName = arcInfo.originVertexName;
            newArcInfo.destinationVertexName = arcInfo.destinationVertexName;
            
            arcs.insert(pair<string,ArcInformation<Resource> >(newArcInfo.arcName, newArcInfo));
            
            
            // update vertices
            vertices.find(newArcInfo.originVertexName)->second.outgoingArcs.push_back(newArcInfo.arcName);
            if (vertices.find(newArcInfo.originVertexName)->second.isReachableFromOrigin) {
                updateVertexReachabilityFromOriginToTrue(newArcInfo.destinationVertexName);
            }
            
            vertices.find(newArcInfo.destinationVertexName)->second.incomingArcs.push_back(newArcInfo.arcName);
            if (vertices.find(newArcInfo.destinationVertexName)->second.isReachableFromDestination) {
                updateVertexReachabilityFromDestinationToTrue(newArcInfo.originVertexName);
            }
            
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::updateVertexReachabilityFromOriginToTrue(string vertexName){
        
        map<string, VertexInformation>::iterator vertexIterator = vertices.find(vertexName);
        if (vertexIterator == vertices.end()) {
            throw "no vertex with this name";
        }
        
        if (!vertexIterator->second.isReachableFromOrigin) {
            vertexIterator->second.isReachableFromOrigin = true;
            for (list<string>::iterator it = vertexIterator->second.outgoingArcs.begin(); it != vertexIterator->second.outgoingArcs.end(); ++it) {
                updateVertexReachabilityFromOriginToTrue(arcs.find(*it)->second.destinationVertexName);
            }
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::updateVertexReachabilityFromDestinationToTrue(string vertexName){
        
        map<string, VertexInformation>::iterator vertexIterator = vertices.find(vertexName);
        if (vertexIterator == vertices.end()) {
            throw "no vertex with this name";
        }
        
        if (!vertexIterator->second.isReachableFromDestination) {
            vertexIterator->second.isReachableFromDestination = true;
            for (list<string>::iterator it = vertexIterator->second.incomingArcs.begin(); it != vertexIterator->second.incomingArcs.end(); ++it) {
                updateVertexReachabilityFromDestinationToTrue(arcs.find(*it)->second.originVertexName);
            }
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::addOriginAndDestinationVertices(){
        // Origin vertex :
        NewVertexInformation OriginInfo;
        OriginInfo.vertexName = "origin";
        OriginInfo.vertexType = SOURCE;
        addVertexIfNotPresent(OriginInfo);
        updateVertexReachabilityFromOriginToTrue("origin");
        
        for (map<string, VertexInformation>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
            if (it->second.vertexType == SOURCE) {
                //I need a dummy arc
                NewArcInformation newArcInfo;
                
                newArcInfo.originVertexName = "origin";
                newArcInfo.destinationVertexName = it->second.vertexName;
                
                newArcInfo.arcName = "dummy_origin_to_" + it->second.vertexName;
                newArcInfo.arcContextName = context_m->getOrAddDummyArcFromOriginName();
                
                addArc(newArcInfo);
            }
        }
        
        
        NewVertexInformation destinationInfo;
        destinationInfo.vertexName = "destination";
        destinationInfo.vertexType = SINK;
        addVertexIfNotPresent(destinationInfo);
        updateVertexReachabilityFromDestinationToTrue("destination");
        
        // Destination vertex
        for (map<string, VertexInformation>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
            if (it->second.vertexType == SINK) {
                //I need a dummy arc
                NewArcInformation newArcInfo;
                
                newArcInfo.originVertexName = it->second.vertexName;
                newArcInfo.destinationVertexName = "destination";
                
                newArcInfo.arcName = "dummy_" + it->second.vertexName + "_to_destination";
                newArcInfo.arcContextName = context_m->getOrAddDummyArcToDestinationName();
                
                addArc(newArcInfo);
            }
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::removeVerticesUnreachableFromOriginOrDestination(){
        for (map<string, VertexInformation>::iterator vertexIterator = vertices.begin(); vertexIterator != vertices.end(); ++vertexIterator) {
            if (!(vertexIterator->second.isReachableFromDestination && vertexIterator->second.isReachableFromOrigin)) {
//                vertices.erase(vertexIterator);
                deleteVertex(vertexIterator->second.vertexName);
            }
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::renameArcContextsIDifTheirExtremitiesAreNotReachables(){
        for (typename map<string, ArcInformation<Resource> >::iterator it = arcs.begin(); it != arcs.end(); ++it) {
            map<string, VertexInformation>::iterator vertexIter = vertices.find(it->second.originVertexName);
            bool isOriginReachable = vertexIter->second.isReachableFromOrigin && vertexIter->second.isReachableFromDestination;
            vertexIter = vertices.find(it->second.destinationVertexName);
            bool isDestinationReachable =  vertexIter->second.isReachableFromOrigin && vertexIter->second.isReachableFromDestination;
            
            if (!isOriginReachable && !isDestinationReachable){
                it->second.arcContext_m->setID(arcContextIDarcWithOriginAndDestinationUnreachable);
            } else if (!isOriginReachable) {
                it->second.arcContext_m->setID(arcContextIDarcWithOriginUnreachable);
            } else if (!isDestinationReachable) {
                it->second.arcContext_m->setID(arcContextIDarcWithDestinationUnreachable);
            }
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::deleteVertex(string vertexName){
        
        map<string, VertexInformation>::iterator vertexIterator = vertices.find(vertexName);
        if (vertexIterator != vertices.end()) {
            for (list<string>::iterator it = vertexIterator->second.outgoingArcs.begin(); it != vertexIterator->second.outgoingArcs.end(); ++it) {
                deleteArc(*it);
//                arcs.erase(*it);
            }
            for (list<string>::iterator it = vertexIterator->second.incomingArcs.begin(); it != vertexIterator->second.incomingArcs.end(); ++it) {
                deleteArc(*it);
//                arcs.erase(*it);
            }
            vertices.erase(vertexIterator);
        }
    }
    
    template<typename Resource>
    void GraphInContextBuilder<Resource>::deleteArc(string arcName){
        
        typename map<string, ArcInformation<Resource> >::iterator arcIterator = arcs.find(arcName);
        if (arcIterator != arcs.end()) {
            map<string, VertexInformation>::iterator vertexIterator = vertices.find(arcIterator->second.originVertexName);
            vertexIterator->second.outgoingArcs.remove(arcName);
            vertexIterator = vertices.find(arcIterator->second.destinationVertexName);
            vertexIterator->second.outgoingArcs.remove(arcName);
            arcs.erase(arcIterator);
        }
    }
    
    
    template<typename Resource>
    map<string, VertexInformation> GraphInContextBuilder<Resource>::getVertices() const{
        return vertices;
    }
    
    template<typename Resource>
    map<string, ArcInformation<Resource> > GraphInContextBuilder<Resource>::getArcs() const{
        return arcs;
    }
    
// Namespace end
}