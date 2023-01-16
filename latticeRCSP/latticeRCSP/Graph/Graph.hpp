//
//  Graph.cpp
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "Graph.h"
#include "../Tools/Console.h"
#include <fstream>

//-------------------------------------------------------
// Constructor and initializers
//-------------------------------------------------------

namespace latticeRCSP{
    
    template <typename Resource>
    Graph<Resource>::Graph(){
        nVertices = 0;
        nArcs = 0;
        isGraphReverse = false;
    }
    
    template <typename Resource>
    Graph<Resource>::Graph(bool isReverse){
        isGraphReverse = false;
        nVertices = 0;
        nArcs = 0;
        isGraphReverse = isReverse;
    }
    
    template <typename Resource>
    Graph<Resource>::Graph(Graph<Resource> const * g){
        nVertices = g->getNVertices();
        Arcs = g->getArcs();
        nArcs = g->getNArcs();
        isOriginDestinationSet = g->getIsOriginAndDestinationSet();
        isGraphReverse = g->getIsReverse();
        if (isOriginDestinationSet) {
            origin = g->getOrigin_withoutDirection();
            destination = g->getDestination_withoutDirection();
        }
        initialize();
    }
    
    template<typename Resource>
    Graph<Resource>::Graph(GraphInContextBuilder<Resource> const & builder){
        isGraphReverse = false;

        map<string, VertexInformation> verticesInfo = builder.getVertices();
        map<string, ArcInformation<Resource> > arcsInfo = builder.getArcs();
        
        // Affect an int to each vertex
        map<string, int> verticesNameToInt;
        int vertexCount = 0;
        isOriginDestinationSet = true;
        for (map<string,VertexInformation>::iterator it = verticesInfo.begin(); it != verticesInfo.end(); ++it) {
            verticesNameToInt.insert(pair<string,int>(it->second.vertexName,vertexCount));
            if (it->second.vertexType == SOURCE) {
                origin.insert(vertexCount);
            } else if (it->second.vertexType == SINK){
                destination.insert(vertexCount);
            }
            vertexCount++;
        }
        
        nVertices = vertexCount;
        
        // Build the arcs
        int arcCount =0;
        nArcs = arcsInfo.size();
        Arcs = vector<Arc<Resource> >(nArcs);
        for (typename map<string, ArcInformation<Resource> >::iterator it = arcsInfo.begin(); it != arcsInfo.end(); ++it) {
            int ori = verticesNameToInt.find(it->second.originVertexName)->second;
            int dest = verticesNameToInt.find(it->second.destinationVertexName)->second;
            Arcs[arcCount]  = Arc<Resource>(it->second.arcContext_m, ori, dest);
            arcCount++;
        }
                
        //Initialize
        initialize();

    }
    
    template <typename Resource>
    Graph<Resource>::Graph(list<pair<int, int> > const & arcList, vector<ArcContext<Resource> * > * pArcContextVector, bool isODSet, int ori, int dest){
        isGraphReverse = false;

        nArcs = arcList.size();
        Arcs = vector<Arc<Resource> >(nArcs);
        
        int i = 0;
        for (list<pair<int, int> >::const_iterator it = arcList.begin(); it != arcList.end() ; ++it) {
            Arcs[i] = Arc<Resource>((*pArcContextVector)[i], it->first, it->second);
            i++;
        }
        
        if (isODSet) {
            setOriginDestination_withoutDirection(ori, dest);
        }
        
        initialize();
    }
    
//    template <typename Resource>
//    Graph<Resource>::Graph(randGraphReader rand){
//        isGraphReverse = false;
//        list<pair<int, int> > edges = rand.getEdges();
//        nArcs = rand.getNEdges();
//        Arcs = vector<pair<int, int> >(nArcs);
//        int compt = 0;
//        for (list<pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it) {
//            Arcs[compt].first = it->first;
//            Arcs[compt].second = it->second;
//            compt++;
//        }
//        
//        initialize();
//    }
    
    template <typename Resource>
    Graph<Resource>::Graph(int nV, list<pair<int, int> > & A){
        isGraphReverse = false;

        nVertices = nV;
        nArcs = A.size();
        Arcs = vector<pair<int, int> >(nArcs);
        int i = 0;
        for (list<pair<int, int> >::iterator it = A.begin(); it !=  A.end(); ++it) {
            Arcs[i] = *it;
            i++;
        }
        initialize();
    }
    
    /*
     Graph<Resource>::Graph(instanceResourceReader & read){
     list<pair<int, int> > arcList = read.getArcs();
     
     isGraphReverse = false;
     nArcs = arcList.size();
     Arcs = vector<pair<int, int> >(nArcs);
     int compt = 0;
     for (list<pair<int, int> >::iterator it = arcList.begin(); it != arcList.end(); ++it) {
     Arcs[compt].first = it->first;
     Arcs[compt].second = it->second;
     compt++;
     }
     initialize();
     }
     */
    
    template <typename Resource>
    Graph<Resource>::Graph(vector<pair<int, int> > arcVector){
        isGraphReverse = false;
        Arcs = arcVector;
        
        initialize();
    }
    
    template <typename Resource>
    void Graph<Resource>::initialize(){
        isArcNameSet = false;
        nArcs = Arcs.size();
        initializeVertices();
        initializeDelta();
        initializeSourceAndSink();
        initializeCyclicOrTopologicalOrder();
    }
    
    template <typename Resource>
    void Graph<Resource>::printToFile(string const & filename) const {
    	ofstream write(filename.c_str());
    	write << nVertices << " vertices " << nArcs << " arcs" << endl;
    	write << "Origins:";
    	for (set<int>::const_iterator it = origin.begin(); it != origin.end(); ++it){
    		write << " " << *it;
    	}
    	write << endl << "Destinations:";
    	for (set<int>::const_iterator it = destination.begin(); it != destination.end(); ++it){
			write << " " << *it;
		}
    	write << endl << "Arcs:" << endl;
    	for (int i = 0; i< nArcs; ++i){
    		write << Arcs[i].getArcContext()->getID() << " ";
    		write << Arcs[i].getOrigin() << " ";
    		write << Arcs[i].getDestination() << endl;
    	}
    	write.close();
    }

    template <typename Resource>
    void Graph<Resource>::initializeVertices(){
        nVertices = 0;
        for (int a = 0; a <nArcs; a++) {
            nVertices = max(nVertices,Arcs[a].getOrigin() + 1);
            nVertices = max(nVertices,Arcs[a].getDestination() + 1);
        }
        
        vector<bool> reachable(nVertices,false);
        for (int a = 0; a <nArcs; a++) {
            if (Arcs[a].getOrigin() < 0 || Arcs[a].getDestination() < 0) throw "Negative arc number are not accepted";
            reachable[Arcs[a].getOrigin()] = true;
            reachable[Arcs[a].getDestination()] = true;
        }
        for (int v= 0; v<nVertices; v++) {
            if (!reachable[v]) {
               throw "v is not reachable";
            }
        }
        
    }
    
    template <typename Resource>
    void Graph<Resource>::initializeDelta(){
        deltaP = vector<vector<int> >(nVertices,vector<int>(0));
        deltaM = vector<vector<int> >(nVertices,vector<int>(0));
        deltaPvert = vector<vector<int> >(nVertices,vector<int>(0));
        deltaMvert = vector<vector<int> >(nVertices,vector<int>(0));
        
        for (int a = 0; a<nArcs; a++) {
            deltaP[Arcs[a].getOrigin()].push_back(a);
            deltaM[Arcs[a].getDestination()].push_back(a);
            deltaPvert[Arcs[a].getOrigin()].push_back(Arcs[a].getDestination());
            deltaMvert[Arcs[a].getDestination()].push_back(Arcs[a].getOrigin());
        }
    }
    
    template <typename Resource>
    void Graph<Resource>::initializeSourceAndSink(){
        source.clear();
        sink.clear();
        vType = vector<int>(nVertices,0);
        for (int v = 0; v<nVertices; v++) {
            if (deltaM[v].size() == 0) {
                vType[v] = 1;
                source.insert(v);
            }
            else if (deltaP[v].size() == 0){
                vType[v] = -1;
                sink.insert(v);
            }
        }
    }
    
    template <typename Resource>
    void Graph<Resource>::orderVertex(int v, int& comptOrder, const vector<bool> &isArcAlreadyOrdered, list<int> &arcsToBeTreated){
        
        bool allIncomingArcsTreated = true;
        for (int a=0; a<deltaM[v].size(); a++) allIncomingArcsTreated &= isArcAlreadyOrdered[deltaM[v][a]];
        
        if (allIncomingArcsTreated) {
            topologicOrderOfV[v] = comptOrder;
            topologicOrder[comptOrder] = v;
            comptOrder++;
            insertVectorToListBack(deltaP[v], arcsToBeTreated);
        }
    }
    
    template <typename Resource>
    void Graph<Resource>::initializeCyclicOrTopologicalOrder(){
        try {
            initializeTopologicalOrder();
            isAcyclic = true;
        } catch (const int a) {
        	auto logger = Console::getPtrLogger();
            logger->info("Graph has cycles, Arc or vertex {} is on a cycle", a);
            isAcyclic = false;
            topologicOrder.clear();
            topologicOrderOfV.clear();
        }
    }
    
    template <typename Resource>
    void Graph<Resource>::initializeTopologicalOrder(){
        
        topologicOrder = vector<int>(nVertices,-1);
        topologicOrderOfV = vector<int>(nVertices,-1);
        
        vector<bool> isArcsAlreadyOrdered = vector<bool>(nArcs,false);
        list<int> arcsToBeTreated ;
        
        // Breadth first search
        int comptOrder = 0;
        for (set<int>::iterator s = source.begin(); s != source.end(); ++s) {
            orderVertex(*s, comptOrder, isArcsAlreadyOrdered, arcsToBeTreated);
        }
        while (!arcsToBeTreated.empty()) {
            int a = arcsToBeTreated.front();
            arcsToBeTreated.pop_front();
            if (isArcsAlreadyOrdered[a]) {
            	throw a;
            }
            isArcsAlreadyOrdered[a] = true;
            orderVertex(Arcs[a].getDestination(), comptOrder, isArcsAlreadyOrdered, arcsToBeTreated);
        }
        
        for (int v = 0; v<nVertices; v++) {
            if (topologicOrder[v] == -1) {
            	throw v;
            }
        }
    }
    
    //-------------------------------------------------------
    // Getters and setters related to orientation
    //-------------------------------------------------------
    
    
//    template <typename Resource>
//    string Graph<Resource>::getArcName(int arc) const{
//        if (isArcNameSet) {
//            return arcName[arc];
//        } else return generateArcNameFromArcIndex(arc);
//    }
//    
//    string Graph<Resource>::generateArcNameFromArcIndex(int arc) const {
//        return "A" + to_string(arc);
//    }
    
    
    template <typename Resource>
    vector<int> Graph<Resource>::getTopologicalOrder() const {
        return topologicOrder;
    }
    
    template <typename Resource>
    bool Graph<Resource>::getIsReverse() const {
        return isGraphReverse;
    }
    
    template <typename Resource>
    void Graph<Resource>::reverseGraph(){
        isGraphReverse = !isGraphReverse;
    }
    
    template <typename Resource>
    bool Graph<Resource>::getIsAcyclic() const {
        return isAcyclic;
    }
    
    template <typename Resource>
    int Graph<Resource>::getNVertices() const{
        return nVertices;
    }
    
    template <typename Resource>
    int Graph<Resource>::getNArcs() const{
        return nArcs;
    }
    
    template <typename Resource>
    Arc<Resource> Graph<Resource>::getArc(string arcID) const{
        Arc<Resource> solution = Arc<Resource>();
        int nSolutionsFound = 0;
        for (int a = 0; a < nArcs; a++) {
            if (Arcs[a].getArcContext()->getID() == arcID) {
                nSolutionsFound++;
                solution = Arcs[a];
            }
        }
        if (nSolutionsFound > 1) {
            throw "More than one arc found || Warning: this can happen in state graphs...";
        }
        if (nSolutionsFound == 0) {
            throw "no arc with this id";
        }
        return solution;
    }
    
    template <typename Resource>
    Arc<Resource> Graph<Resource>::getArc(int arc) const{
        return Arcs[arc];
    }
    
//    template <typename Resource>
//    void Graph<Resource>::setArcs(list<pair<int, int> > & l){
//        if (l.size() != nArcs) {
//            cout << "Problem of length" << endl;
//        }
//        
//        Arcs = vector<pair<int, int> >(nArcs,pair<int, int>(-1,-1));
//        int i = 0;
//        for (list<pair<int, int> >::iterator it = l.begin(); it != l.end(); ++it) {
//            if (isGraphReverse) {
//                Arcs[i].first = it->second;
//                Arcs[i].second = it->first;
//            } else {
//                Arcs[i].first = it->first;
//                Arcs[i].second = it->second;
//            }
//            i++;
//        }
//    }
    
//    template <typename Resource>
//    int Graph<Resource>::arcOrign(int a){
//        if (isGraphReverse) return Arcs[a].getDestination();
//        else return Arcs[a].getOrigin();
//    }
//
//    template <typename Resource>
//    int Graph<Resource>::arcDestination(int a){
//        if (!isGraphReverse) return Arcs[a].getDestination();
//        else return Arcs[a].getOrigin();
//    }
    
    template <typename Resource>
    vector<int> const & Graph<Resource>::deltaPa(int v){
        if (isGraphReverse) return deltaM[v];
        else return deltaP[v];
    }
    
    template <typename Resource>
    vector<int> const & Graph<Resource>::deltaMa(int v){
        if (!isGraphReverse) return deltaM[v];
        else return deltaP[v];
    }
    
    template <typename Resource>
    vector<int> const & Graph<Resource>::deltaPv(int v){
        if (isGraphReverse) return deltaMvert[v];
        else return deltaPvert[v];
    }
    
    template <typename Resource>
    vector<int> const & Graph<Resource>::deltaMv(int v){
        if (!isGraphReverse) return deltaMvert[v];
        else return deltaPvert[v];
    }
    
    template <typename Resource>
    set<int> const & Graph<Resource>::getSource() const{
        return source;
    }
    
    template <typename Resource>
    set<int> const & Graph<Resource>::getSink() const{
        return sink;
    }
    
    template <typename Resource>
    void Graph<Resource>::setOriginDestination_withoutDirection(int o, int d){
        isOriginDestinationSet = true;
        origin.clear();
        origin.insert(o);
        destination.clear();
        destination.insert(d);
        isOriginDestinationSet = true;
    }
    
    
    template <typename Resource>
    void Graph<Resource>::setOriginDestination_withoutDirection(set<int> o, set<int> d){
        origin = o;
        destination = d;
        isOriginDestinationSet = true;
    }
    
    template <typename Resource>
    void Graph<Resource>::setOriginDestination_withDirection(set<int> o, set<int> d){
        if (isGraphReverse) {
            setOriginDestination_withoutDirection(d, o);
        } else {
            setOriginDestination_withoutDirection(o, d);
        }
    }
    
    template <typename Resource>
    void Graph<Resource>::setSourceAndSinkAsOriginAndDestination(){
        if (source.empty() || sink.empty()) {
            throw "Sources or sink are empty";
        }
        origin = source;
        destination = sink;
        
    }
    
    template <typename Resource>
	void Graph<Resource>::addOrigin(int o){
    	if (isGraphReverse) destination.insert(o);
    	else origin.insert(o);
    }

    template <typename Resource>
	void Graph<Resource>::addDestination(int d){
    	if (isGraphReverse) origin.insert(d);
    	else destination.insert(d);
    }

    template <typename Resource>
    bool Graph<Resource>::getIsOriginAndDestinationSet() const{
        return isOriginDestinationSet;
    }
    
    template <typename Resource>
    set<int> Graph<Resource>::getOrigin_withoutDirection() const{
        return origin;
    }
    
    template <typename Resource>
    set<int> Graph<Resource>::getDestination_withoutDirection() const{
        return destination;
    }
    
    template <typename Resource>
    vector<Arc<Resource> > Graph<Resource>::getArcs() const{
        return Arcs;
    }
    
    template <typename Resource>
    set<int> & Graph<Resource>::getOrigin(){
        if (!isGraphReverse) return origin;
        else return destination;
    }
    
    template <typename Resource>
    set<int> & Graph<Resource>::getDestination(){
        if (isGraphReverse) return origin;
        else return destination;
    }
    
    template <typename Resource>
        set<int> const & Graph<Resource>::getConstDestination() const{
            if (isGraphReverse) return origin;
            else return destination;
        }

    template <typename Resource>
    int Graph<Resource>::vertexType(int v){
        if (isGraphReverse) return -vType[v];
        else return vType[v];
    }
    
    template <typename Resource>
    int Graph<Resource>::topologicalOrderOfV(int v){
        if (isGraphReverse) return nVertices - 1 - topologicOrderOfV[v];
        else return topologicOrderOfV[v];
    }
    
    template <typename Resource>
    int Graph<Resource>::topologicalOrder(int i){
        if (isGraphReverse) return topologicOrder[nVertices - 1 - i];
        else return topologicOrder[i];
    }
    
    template <typename Resource>
    int Graph<Resource>::arcDestination(int arc) const{
        if (Graph<Resource>::getIsReverse()) return  Graph<Resource>::Arcs[arc].getOrigin();
        else return Graph<Resource>::Arcs[arc].getDestination();
    }
    
    template <typename Resource>
    int Graph<Resource>::arcOrigin(int arc) const{
        if (!Graph<Resource>::getIsReverse()) return Graph<Resource>::Arcs[arc].getOrigin();
        else return Graph<Resource>::Arcs[arc].getDestination();
    }
    
    template <typename Resource>
    template<typename morphismToRFunctor>
    vector<double> Graph<Resource>::directionDijkstraAlgorithmToAllVertices(){
        multimap<double,int> candidateVertices; // Vertices candidate for entrance
        vector<double> shortestPathFromOriginLength(nVertices,-1);
        set<int> orig = getOrigin();
        for (set<int>::const_iterator it = orig.begin(); it!= orig.end(); ++it) {
            candidateVertices.insert(pair<double, int>(0,*it));
        }
        
        while (!candidateVertices.empty()) {
            // get the nearest neighboor
            multimap<double, int>::iterator beg = candidateVertices.begin();
            int currentVertex = beg->second;
            double currentCost = beg->first;
            candidateVertices.erase(beg);
            
            if (shortestPathFromOriginLength[currentVertex] < 0) { // if vertex is not in the active subgraph, add the vertex
                shortestPathFromOriginLength[currentVertex] = currentCost;
                
                // update the descendants
                for (int da =0; da < deltaPa(currentVertex).size(); da++) {
                    int a = deltaPa(currentVertex)[da];
                    int successor = arcDestination(a);
                    if (shortestPathFromOriginLength[successor] < 0) {
                        candidateVertices.insert(pair<double, int>(currentCost + morphismToRFunctor::getKey(Arcs[a].getResource()),successor));
                    }
                }
            }
        }
        return shortestPathFromOriginLength;
    }
    
    template <typename Resource>
	template<typename morphismToRFunctor>
	vector<double> Graph<Resource>::directionAcyclicDynamicProgrammingToAllVertices(){
        vector<double> shortestPathFromOriginLength(nVertices,std::numeric_limits<double>::infinity());
        set<int> orig = getOrigin();
        for (auto && vert : getOrigin()){
        	shortestPathFromOriginLength[vert] = 0;
        }
        for (int i = 0; i < nVertices; ++i) {
			int vert = topologicalOrder(i);
			for (auto && arc : deltaMa(vert)){
				double arcValue = shortestPathFromOriginLength[arcOrigin(arc)];
				arcValue += morphismToRFunctor::getKey(Arcs[arc].getResource());
				if (arcValue < shortestPathFromOriginLength[vert]){
					shortestPathFromOriginLength[vert] = arcValue;
				}
			}
		}
        return shortestPathFromOriginLength;
    }
    
    //-------------------------------------------------------
    // Auxiliaries
    //-------------------------------------------------------
    
    
    template <typename Resource>
    vector<int> Graph<Resource>::concatenate(vector<int> A,vector<int> B){
        vector<int> AB;
        AB.reserve( A.size() + B.size() ); // preallocate memory
        AB.insert( AB.end(), A.begin(), A.end() );
        AB.insert( AB.end(), B.begin(), B.end() );
        return AB;
    }
    
    template <typename Resource>
    vector<vector<int> > Graph<Resource>::concatenate(vector<vector<int> > A, vector<vector<int> > B){
        vector<vector<int> > AB;
        AB.reserve( A.size() + B.size() ); // preallocate memory
        AB.insert( AB.end(), A.begin(), A.end() );
        AB.insert( AB.end(), B.begin(), B.end() );
        return AB;
    }
    
    
    template <typename Resource>
    void Graph<Resource>::insertVectorToListBack(const vector<int> &vect, list<int> &l){
        for (int i =0; i<vect.size();i++) {
            l.push_back(vect[i]);
        }
    }
    
//    template <typename Resource>
//    void Graph<Resource>::setArcVector(){
//        Arcs = vector<pair<int, int> >(nArcs,pair<int, int>(-1,-1));
//    }
//    
//    template <typename Resource>
//    void Graph<Resource>::setArc(int arc, int originVert, int destinationVert){
//        if (isGraphReverse){
//            Arcs[arc].first = destinationVert;
//            Arcs[arc].second = originVert;
//            
//        } else {
//            Arcs[arc].first = originVert;
//            Arcs[arc].second = destinationVert;
//        }
//    }
}
