//
//  Graph.h
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__Graph__
#define __RCSP__Graph__

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <map>

#include "Arc.h"
#include "../GraphBuilder/GraphInContextBuilder.h"

using namespace std;

namespace latticeRCSP {
    
    template<typename Resource>
    class GraphInContextBuilder;
    
    template<typename Resource>
    class Graph {
    private:
        // everything related to Graph orintation is private
        bool isGraphReverse;
        
        
        vector<vector<int> > deltaP;
        vector<vector<int> > deltaM;
        vector<vector<int> > deltaPvert;
        vector<vector<int> > deltaMvert;
        
        // Origin and destination
        set<int> source;
        set<int> sink;
        vector<int> vType;
        
        bool isOriginDestinationSet;
        set<int> origin;
        set<int> destination;
        
        void initializeTopologicalOrder();
        vector<int> topologicOrder;
        vector<int> topologicOrderOfV;
        
    protected:
        int nVertices;
        int nArcs;
        vector<Arc<Resource> > Arcs;

        
        bool isAcyclic;
        
        bool isArcNameSet;
        vector<string> arcName;
        
//        void setArcVector();
//        void setArc(int arc, Arc<Resource> &);
        
        //---------------------------------------------------
        // Getters and setters related to orientation are protected
        //---------------------------------------------------
//    public:
//        int arcOrign(int);
//        int arcDestination(int);
        
    public:
        vector<int> const & deltaPa(int);
        vector<int> const & deltaMa(int);
        vector<int> const & deltaPv(int);
        vector<int> const & deltaMv(int);
        
    public:
        bool getIsOriginAndDestinationSet() const;
        set<int> & getOrigin();
        set<int> & getDestination();
        set<int> const & getConstDestination() const;
        int vertexType(int);
        
        
        int topologicalOrderOfV(int);
        int topologicalOrder(int); // gives the vertex at the int position in the topological order
        
    protected:
        //---------------------------------------------------
        // Initialization functions
        //---------------------------------------------------
        void initializeVertices();
        void initializeDelta();
        void initializeCyclicOrTopologicalOrder();
        void initializeSourceAndSink();
        void orderVertex(int v, int& comptOrder, const vector<bool> &isArcAlreadyOrdered, list<int> &arcsToBeTreated);
        
    public:
        //---------------------------------------------------
        // Constructor
        //---------------------------------------------------
        Graph();
        Graph(bool);
        Graph(GraphInContextBuilder<Resource> const &);
        Graph(list<pair<int, int> > const &, vector<ArcContext<Resource> * > *, bool isOriginDestinationSet = false, int origin = -1, int destination = -1);
        //Graph(randGraphReader);
        Graph(int nVertices, list<pair<int, int> > &);
        //Graph(instanceResourceReader &);
        Graph(vector<pair<int, int> >);
        Graph(const Graph *);
        
        void initialize();
        void printToFile(string const &) const;
        
        //---------------------------------------------------
        // Getters and setters
        //---------------------------------------------------
        
        //string getArcName(int arc) const;
        //string generateArcNameFromArcIndex(int arc) const;
        Arc<Resource> getArc(int arc) const;
        Arc<Resource> getArc(string arcID) const;
        
        bool getIsReverse() const;
        void reverseGraph();
        
        bool getIsAcyclic() const;
        vector<int> getTopologicalOrder() const;

        
        int arcOrigin(int) const;
        int arcDestination(int) const;
        
        int getNVertices() const;
        int getNArcs() const;
        
        vector<Arc<Resource> > getArcs() const;
        set<int> getOrigin_withoutDirection() const;
        set<int> getDestination_withoutDirection() const;
        
        set<int> const & getSource() const;
        set<int> const & getSink() const;
        
        void setOriginDestination_withoutDirection(int, int);
        void setOriginDestination_withoutDirection(set<int>, set<int>);
        void setOriginDestination_withDirection(set<int>, set<int>);
        void setIsOriginDestinationSet(bool b){isOriginDestinationSet = b;}
        
        void setSourceAndSinkAsOriginAndDestination();
        
        void addOrigin(int o);
        void addDestination(int d);
        
        //void setArcs(list<pair<int,int> > &);
        
        // Topological order
        
        
        // Plot Graph dot
        
        // Other
        template<typename morphismToRFunctor>
        vector<double> directionDijkstraAlgorithmToAllVertices();
        
        template<typename morphismToRFunctor>
        vector<double> directionAcyclicDynamicProgrammingToAllVertices();

        //-------------------------------------------------------
        // Auxiliaries
        //-------------------------------------------------------
        static void insertVectorToListBack(const vector<int> &A, list<int> &L);
        static vector<int> concatenate(vector<int> A,vector<int> B);
        static vector<vector<int> > concatenate(vector<vector<int> > A,vector<vector<int> > B);
    };
}

#include "Graph.hpp"

#endif /* defined(__RCSP__Graph__) */
