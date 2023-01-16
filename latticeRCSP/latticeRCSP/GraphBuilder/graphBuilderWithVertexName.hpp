//
//  graphBuilderWithVertexName.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 30/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "graphBuilderWithVertexName.h"

namespace latticeRCSP{
    template <typename resource>
    graphBuilderWithVertexName<resource>::graphBuilderWithVertexName(){
        nVertices = 0;
        nArcs = 0;
    }
    
//    template<typename resource>
//    int graphBuilderWithVertexName<resource>::getOrCreateVertex(string v){
//        map<string,int>::iterator itVertex = vertices.find(v);
//        if (itVertex == vertices.end()) {
//            itVertex = vertices.insert(pair<string,int>(v,nVertices));
//            nVertices++;
//        }
//        return itVertex->second;
//    }
    template <typename resource>
    int graphBuilderWithVertexName<resource>::insertVertex(resource & r, int type){
        vertexInformation vI;
        vI.name = r.getName();
        vI.type = type;
        vI.index = nVertices;
        nVertices++;
        
        vertices.insert(pair<string,pair<vertexInformation,resource> >(r.getName(),pair<vertexInformation,resource>(vI, r)));
        
        return nVertices - 1;
    }
    
    template <typename resource>
    int graphBuilderWithVertexName<resource>::insertArc(string o, string d, resource & r,bool isInterDuty){
        
        if (vertices.find(o) == vertices.end() || vertices.find(d) == vertices.end()) {
            throw "Arc between non existing vertices";
            // NOTA BENE : if one of the vertices of the arc does not exist, an exception is thrown
        }
        
        arcInformation aI;
        aI.originName = o;
        aI.destinationName = d;
        aI.isInterDuty = isInterDuty;
        
        Arcs.push_back(pair<int,pair<arcInformation,resource> >(nArcs,pair<arcInformation,resource>(aI,r)));
        nArcs++;
        return nArcs -1;
    }
    
    template <typename resource>
    void graphBuilderWithVertexName<resource>::initializeDelta(){
        for (map<int,pair<arcInformation,resource> > it = Arcs.begin(); it != Arcs.end(); ++it) {
            vertices.find(it->second.first.originVertex)->second.first.deltaP.push_back(it->first);
            vertices.find(it->second.first.destinationVertex)->second.first.deltaM.push_back(it->first);
        }
    }
    
    template <typename resource>
    void graphBuilderWithVertexName<resource>::deleteVertex(string vertex){
        typename map<string,pair<vertexInformation,resource> >::iterator it = vertices.find(vertex);
        if (it != vertices.end()) {
            for (int a = 0; a < it->second.first.deltaM.size(); ++a) {
                Arcs.erase(it->second.first.deltaM[a]);
                nArcs--;
            }
            for (int a = 0; a < it->second.first.deltaP.size(); ++a) {
                Arcs.erase(it->second.first.deltaP[a]);
                nArcs--;
            }
            vertices.erase(it);
            nVertices--;
        }
    }
    
    template <typename resource>
    void graphBuilderWithVertexName<resource>::addOriginAndDestination(){
        // Insert origin ------------------------
        originVertex = "origin";
        vertexInformation vI;
        vI.name = "";
        vI.type = -2;
        vI.index = nVertices;
        nVertices++;
        resource r = resource();
        typename map<string,pair<vertexInformation,resource> >::iterator itOrigin;
        itOrigin = vertices.insert(pair<string,pair<vertexInformation,resource> >(originVertex,pair<vertexInformation, resource>(vI,r)));
        
        typename map<string,pair<vertexInformation,resource> >::iterator it;
        for (it = vertices.begin(); it != vertices.end(); ++it) {
            if (it->second.first.type == -1) {
                insertArc(originVertex, it->first, resource(), false);
            }
        }
        
        // Insert destination ------------------------
        destinationVertex = "destination";
        vI.type = 2;
        vI.index = nVertices;
        nVertices++;
        vertices.insert(pair<string,pair<vertexInformation,resource> >(destinationVertex,pair<vertexInformation, resource>(vI,r)));
        typename map<string,pair<vertexInformation,resource> >::iterator itDestination;
        itDestination = vertices.insert(pair<string,pair<vertexInformation,resource> >(destinationVertex,pair<vertexInformation, resource>(vI,r)));
        for (it = vertices.begin(); it != vertices.end(); ++it) {
            if (it->second.first.type == 1) {
                insertArc(it->first, destinationVertex, resource(), false);
            }
        }
    }
//    template <typename resource>
//    int graphBuilderWithVertexName<resource>::insertArc(int o, int d, resource r){
//        nArcs++;
//        Arcs.push_back(pair<int,int>(o,d));
//        arcResources.push_back(r);
//        return nArcs -1;
//    }
    
    template<typename resource>
    void graphBuilderWithVertexName<resource>::removeUnreachableVerticesAndArcs(){
        
        // Initialize isReachable to 0 for each vertex -------------
        typename map<string,pair<vertexInformation,resource> >::iterator it;
        for (it = vertices.begin(); it!=vertices.end(); ++it) {
            it->second.first.isReachable = 0;
        }
        
        // Forward search ------------------------------------------
        set<string> verticesToBeInserted;
        verticesToBeInserted.insert(originVertex);
        while (!verticesToBeInserted.empty()) {
            string vertex = *verticesToBeInserted.begin();
            vertices.erase(vertices.begin());
            it = vertices.find(vertex);
            it->second.first.isReachable +=1;
            
            vector<int> del = it->second.first.deltaP;
            for (int a = 0; a < del.size(); a++) {
                string newVertex = Arcs.find(del[a])->second.first.destinationName;
                if (vertices.find(newVertex)->second.first.isReachable % 2 == 0) {
                    verticesToBeInserted.insert(newVertex);
                }
            }
        }
        
        // Backward search ----------------------------------------
        verticesToBeInserted.clear();
        verticesToBeInserted.insert(destinationVertex);
        
        while (!verticesToBeInserted.empty()) {
            string vertex = *verticesToBeInserted.begin();
            vertices.erase(vertices.begin());
            it = vertices.find(vertex);
            it->second.first.isReachable +=2;
            
            vector<int> del = it->second.first.deltaM;
            for (int a = 0; a < del.size(); a++) {
                string newVertex = Arcs.find(del[a])->second.first.originName;
                if ((vertices.find(newVertex)->second.first.isReachable / 2 ) % 2 == 0) {
                    verticesToBeInserted.insert(newVertex);
                }
            }
        }
        
        // Delete unreachable -------------------------------------
        
        for (it = vertices.begin(); it!=vertices.end(); ++it) {
            if (it->second.first.isReachable != 3) {
                deleteVertex(it->first);
            }
        }
        
    }
    

    template<typename resource>
    void graphBuilderWithVertexName<resource>::reIndexVerticesAndArcs(){
        int vertexIndex = 0;
        typename map<string,pair<vertexInformation,resource> >::iterator it;
        for (it = vertices.begin(); it!=vertices.end(); ++it) {
            it->second.first.index = vertexIndex;
            vertexIndex ++;
        }
        if (vertexIndex != nVertices) throw "Aie";
        
        // Beware in the update of arcs : the index is the key. But as it is updated to something smaller : no risk
        int arcIndex = 0;
        
        typename map<int,pair<arcInformation,resource> >::iterator it1 = Arcs.begin();
        typename map<int,pair<arcInformation,resource> >::iterator it2;
        while (it1 != Arcs.end()) {
            it2 = it1;
            it1++;
            it2->first = arcIndex;
            arcIndex++;
        }
        
        initializeDelta();
    }
    
    template <typename resource>
    int graphBuilderWithVertexName<resource>::getNVertices() {
        return nVertices;
    }
    
    template <typename resource>
    vector<pair<int,int> > graphBuilderWithVertexName<resource>::getArcs(){
        vector<pair<int, int> > result(nArcs);
        
        typename map<int,pair<arcInformation,resource> >::iterator it;
        for (it = Arcs.begin(); it != Arcs.end(); ++it) {
            result[it->first].first = vertices.find(it->second.first.originName)->second.first.index;
            result[it->first].second = vertices.find(it->second.first.destinationName)->second.first.index;
        }
        
        return result;
        
    }
    
    template <typename resource>
    vector<resource> graphBuilderWithVertexName<resource>::getArcResources(){
        vector<resource> results(nArcs);
        
        typename map<int,pair<arcInformation,resource> >::iterator it;
        for (it = Arcs.begin(); it != Arcs.end(); ++it) {
            results[it->first].first = vertices.find(it->second.first.originName)->second.resource;
            results[it->first].expand(it->second.second);
        }
        
        return results;
        
    }
    
   
}