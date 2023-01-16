//
//  stateGraph.cpp
//  RCSP
//
//  Created by Axel Parmentier on 14/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "stateGraph.h"
#include "../Resources/ResourceFactory.h"

namespace latticeRCSP {
    template<typename resource, typename networkGraphType>
    stateGraph<resource, networkGraphType>::stateGraph(networkGraphType * net) : resourceGraph<resource>(){
        networkG = net;
        resourceGraph<resource>::isArcIDset = false;
    }
    
    template<typename resource, typename networkGraphType>
    void stateGraph<resource, networkGraphType>::expandResourceAlongArc(int vertex, int state, const resource & r, vector<list<pair<pair<int,int>, resource> > > & vertexIncomingResources){
        for (int a = 0; a<networkG->deltaPa(vertex).size(); a++) {
            resource newR = r;
            newR.expand(networkG->getArcResource(networkG->deltaPa(vertex)[a]));
            vertexIncomingResources[networkG->deltaPv(vertex)[a]].push_back(pair<pair<int,int>, resource>(pair<int,int>(state, networkG->deltaPa(vertex)[a]),newR));
        }
    }
    
    template<typename resource, typename networkGraphType>
    void stateGraph<resource, networkGraphType>::buildStateGraph(int maxStatePerVertex, double periodOfTimeForImprove_s, double minImproveDuringPeriod_percent, double maxTime_s){
        
        if (graph::getIsReverse() != networkG->getIsReverse()) {
            graph::reverseGraph();
        }
        
        isStateGraphReverse = graph::getIsReverse();
        
        vector<list<pair<pair<int,int>, resource> > > vertexIncomingResources(networkG->getNVertices());
        list<resource> stateResource;
        list<pair<int, int> > stateArcsList;
        vector<list<int> > stateArcsOfNetworkArcList(networkG->getNArcs());
        stateVerticesOfVertex = vector<vector<int> >(networkG->getNVertices());
        
        
        cout << endl << "-------------------------------------" << endl << "State graph building" << endl << "-------------------------------------"  << endl;
        clock_t t0 = clock();
        
        graph::nVertices = 0;
        graph::nArcs  = 0;
        
        for (int v = 0; v<networkG->getNVertices(); v++) {
            if (v%20 == 0)  cout << v << " vertices out of " << networkG->getNVertices() << " treated in "<< (clock() - t0)/(float) CLOCKS_PER_SEC << endl;
            int vertex = networkG->topologicalOrder(v);
            if (networkG->vertexType(vertex)== 1) {
                resource r = ResourceFactory<resource>::getStaticNewResource(Graph<resource>::getIsReverse());
                r.setCost(0);
                expandResourceAlongArc(vertex, graph::nVertices, r, vertexIncomingResources);
                stateVerticesOfVertex[vertex] = vector<int>(1,graph::nVertices);
                stateResource.push_back(r);
                graph::nVertices++;
                
            } else {
                vector<resource > newResources;
                vector<int> clusterOfInitial;
                if (vertexIncomingResources[vertex].size()>maxStatePerVertex) {
                    vector<resource const *> pv(vertexIncomingResources[vertex].size());
                    int i = 0;
                    for (typename list<pair<pair<int,int>, resource> >::iterator it = vertexIncomingResources[vertex].begin() ; it != vertexIncomingResources[vertex].end(); ++it ) {
                        pv[i] = &(it->second);
                        i++;
                    }
                    latticeClusterization<resource, latticeContainer<resource> > cluster(maxStatePerVertex,pv);
                    cluster.simulatedAnnealing(periodOfTimeForImprove_s, minImproveDuringPeriod_percent, maxTime_s);
                    clusterOfInitial = cluster.getClusterOfInitial();
                    newResources = cluster.getClustersMeet();
                } else {
                    newResources = vector<resource >(vertexIncomingResources[vertex].size());
                    clusterOfInitial = vector<int>(vertexIncomingResources[vertex].size());
                    int i = 0;
                    for (typename list<pair<pair<int,int>, resource> >::iterator it = vertexIncomingResources[vertex].begin() ; it != vertexIncomingResources[vertex].end(); ++it ) {
                        clusterOfInitial[i] = i;
                        newResources[i] = it->second;
                        i++;
                    }
                
                }
                // build state arcs
                int i = 0;
                for (typename list<pair<pair<int, int>,resource> >::iterator it = vertexIncomingResources[vertex].begin(); it != vertexIncomingResources[vertex].end(); ++it) {
                    stateArcsList.push_back(pair<int, int>(it->first.first,graph::nVertices + clusterOfInitial[i]));
                    stateArcsOfNetworkArcList[it->first.second].push_back(graph::nArcs);
                    i++;
                    graph::nArcs++;
                }
                // build and expand state vertices
                stateVerticesOfVertex[vertex] = vector<int>(newResources.size());
                for (int r = 0; r<newResources.size(); r++) {
                    stateResource.push_back(newResources[r]);
                    stateVerticesOfVertex[vertex][r] = graph::nVertices + r;
                    expandResourceAlongArc(vertex,graph::nVertices + r, newResources[r], vertexIncomingResources);
                }
                graph::nVertices += newResources.size();
                
                
            }
            //vertexIncomingResources[vertex].clear();
        }
        //------------------------------
        // Rebuild the graph
        //------------------------------
        
        // State resources : the follwowing codres insert in resources the stateVerticesLowerBound instead of inserting the resource of the state are
//        resourceGraph<resource>::resources = vector<resource>(graph::nVertices);
//        typename list<resource>::iterator it = stateResource.begin();
//        for (int s = 0; s<graph::nVertices; s++) {
//            resourceGraph<resource>::resources[s] = *it;
//            ++it;
//        }
        
        // Convert the list of state arcs int
        graph::setArcs(stateArcsList);
        stateArcsOfArc = vector<vector<int> >(networkG->getNArcs());
        for (int a = 0; a < networkG->getNArcs(); a++) {
            stateArcsOfArc[a] = vector<int>(stateArcsOfNetworkArcList[a].size());
            int sa = 0;
            for (list<int>::iterator it = stateArcsOfNetworkArcList[a].begin(); it != stateArcsOfNetworkArcList[a].end(); ++it) {
                stateArcsOfArc[a][sa] = *it;
                sa++;
            }
        }
        
        // Initialization of vertex of state and arc of state arc
        vertexOfState = vector<int>(graph::nVertices);
        for (int v = 0; v <networkG->getNVertices(); v++) {
            for (int s = 0; s<stateVerticesOfVertex[v].size(); s++) {
                vertexOfState[stateVerticesOfVertex[v][s]] = v;
            }
        }
        
        arcOfStateArc = vector<int>(graph::nArcs);
        resourceGraph<resource>::resources = vector<resource>(graph::nArcs);
        for (int a = 0; a < networkG->getNArcs(); a++) {
            for (int s = 0; s < stateArcsOfArc[a].size(); s++) {
                arcOfStateArc[stateArcsOfArc[a][s]] = a;
                resourceGraph<resource>::resources[stateArcsOfArc[a][s]] = networkG->getArcResource(a);
            }
        }
        
        
        // Graph initialization
        graph::initialize();
        setRecursiveArcID();
    }
    
    template<typename resource, typename networkGraphType>
    template<typename goodPathSetKeyFunctor>
    vector<BandPSetList<boundAndPaths<resource> > > stateGraph<resource, networkGraphType>::getBoundSet() {
        cout << "Exec stateGraph::getBoundSet()" << endl;
        if (isStateGraphReverse != graph::getIsReverse()) throw "There is only one direction for state graph" ;
        vector<BandPSetList<boundAndPaths<resource> > > result(networkG->getNVertices());
        for (int vertex = 0; vertex < networkG->getNVertices(); vertex++) {
            int nStates = stateVerticesOfVertex[vertex].size();
            vector<boundAndPaths<resource> > boundSets(nStates);
            for (int s = 0; s < nStates; s++) {
                int state = stateVerticesOfVertex[vertex][s];
                result[vertex].insert(boundAndPaths<resource>(resourceGraph<resource>::getLowerBoundOnPathFromOriginToVertex(state), resourceGraph<resource>::template getSetOfGoodPathsFromOriginToVertex<goodPathSetKeyFunctor>(state)));
            }
        }
        return result;
    }
    
    template<typename resource, typename networkGraphType>
    template <typename argumentBoundSet>
    vector<BandPSetList<argumentBoundSet> > stateGraph<resource, networkGraphType>::getBoundSet(vector<argumentBoundSet> & argumentVector) {
        vector<BandPSetList<boundAndPaths<resource> > > result(networkG->getNVertices());
        for (int vertex = 0; vertex < networkG->getNVertices(); vertex++) {
            int nStates = stateVerticesOfVertex[vertex].size();
            vector<boundAndPaths<resource> > boundSets(nStates);
            for (int s = 0; s < nStates; s++) {
                int state = stateVerticesOfVertex[vertex][s];
                result[vertex].insert(argumentVector[state]);
            }
        }
        return result;
    }
    
    template<typename resource, typename networkGraphType>
    int stateGraph<resource, networkGraphType>::getNetworkArcRecursive(int arc) const{
        return networkG->getNetworkArcRecursive(arcOfStateArc[arc]);
    }
    
    template<typename resource, typename networkGraphType>
    void stateGraph<resource, networkGraphType>::setRecursiveArcID(){
        resourceGraph<resource>::isArcIDset = true;
        resourceGraph<resource>::arcID = vector<int>(graph::nArcs);
        for (int arc = 0; arc < graph::nArcs; arc++) {
            resourceGraph<resource>::arcID[arc] = getNetworkArcRecursive(arc);
        }
    }
}




