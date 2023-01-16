//
//  Clusterization.cpp
//  latticeClustering
//
//  Created by Axel Parmentier on 09/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "Clusterization.h"
#include "../Tools/Console.h"


template <typename Resource, typename Cluster>
typename Clusterization<Resource,Cluster>::CostPolicy Clusterization<Resource,Cluster>::costPolicy_s = SUM_OF_CLUSTERS_COST;

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::setCostPolicy(CostPolicy pol){
    costPolicy_s = pol;
}

template <typename Resource, typename Cluster>
Clusterization<Resource,Cluster>::Clusterization(int nCluster, vector<const Resource *> const & b) {
    simulatedAnnealinginitialTemperature = SIMULATED_ANNEALING_INITIAL_TEMPERATURE;
    nClusterWithFixedElements = 0;
    nInitial = b.size();
    nK = nCluster;
    initialElements = b;
    
    if (b.size() < nK + 1) throw "clusterization not needed";
    clusters = vector<Cluster>(nK,Cluster());
    setRandomSolution();
    initializeClustersToSolution();
}

template <typename Resource, typename Cluster>
Clusterization<Resource,Cluster>::Clusterization(int nNewCluster, vector<vector<Resource const *> > const & clustersWithFixedElements, vector<Resource const *> const & newElements){
    simulatedAnnealinginitialTemperature = SIMULATED_ANNEALING_INITIAL_TEMPERATURE;
    nClusterWithFixedElements = clustersWithFixedElements.size();
    nK = nNewCluster + nClusterWithFixedElements;
    nInitial = newElements.size();
    initialElements = newElements;
    
    if (newElements.size() < nNewCluster + 1) throw "clusterization not needed";
    clusters = vector<Cluster>(nK,Cluster());
    initializeFixedCluster(clustersWithFixedElements);
    setRandomSolution();
    initializeClustersToSolution();
    
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::setRandomSolution() {
    clusterOfInitial = vector<int>(nInitial,-1);
    for (int  i = 0; i<nK - nClusterWithFixedElements; i++) {
        clusterOfInitial[i] = i + nClusterWithFixedElements;

    }
    for (int i = nK - nClusterWithFixedElements; i<nInitial; i++) {
        clusterOfInitial[i] = rand() % nK;
    }

}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::initializeClustersToSolution(){
    for (int c = 0; c <clusters.size(); c++) {
        clusters[c].clear();
    }
    for (int i = 0; i<nInitial; i++) {
        clusters[clusterOfInitial[i]].insertElement(i,initialElements[i]);
    }
//    for (int i = 0; i<nK; i++) {
//        clusters[i].computeCost();
//    }
    calculateCost();
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::initializeFixedCluster(const vector<vector<const Resource *> > & fixedClusters){
    for (int c = 0; c < fixedClusters.size(); c++) {
        clusters[c] = Cluster(fixedClusters[c]);
    }
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::setNonAffectedClusters() {
    vector<int> clusterCard(nK,0);
    for (int i =0; i<nInitial; i++) {
        clusterCard[clusterOfInitial[i]]++;
    }
    bool updated = false;
    for (int k = 0; k<nK; k++) {
//        if (clusterCard[k] == 0) {
        if (clusters[k].size_allElements() == 0){
            int i = rand()%nInitial;
            while (!(clusters[clusterOfInitial[i]].size_allElements() > 1 && clusterCard[clusterOfInitial[i]] > 0) ){
                i= rand()%nInitial;
            }
            clusterCard[clusterOfInitial[i]]--;
            clusterCard[k]++;
            clusterOfInitial[i] = k;
            updated = true;
        }
    }
    if (updated) calculateCost();
    
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::calculateCost() {
    // Cost must be always up to date in the Clusters
    cost = 0;
    if (costPolicy_s == SUM_OF_CLUSTERS_COST){
        for (int i=0; i<nK; i++) {
            cost += clusters[i].getCost();
        }
    } else if (costPolicy_s == MAX_OF_CLUSTERS_COST){
        for (int i=0; i<nK; i++) {
            if (cost < clusters[i].getCost()) cost = clusters[i].getCost();
        }
    }
    
}

template <typename Resource, typename Cluster>
double Clusterization<Resource,Cluster>::swap(int firstElement, int secondElement){
    int firstCluster = clusterOfInitial[firstElement];
    double result = addRemove(firstElement, clusterOfInitial[secondElement]);
    result += addRemove(secondElement, firstCluster);
    return result;
}


template <typename Resource, typename Cluster>
double Clusterization<Resource,Cluster>::addRemove(int elem, int newCluster){
    double result = clusters[clusterOfInitial[elem]].removeElement(elem);
    clusterOfInitial[elem] = newCluster;
    result += clusters[newCluster].insertElement(elem,initialElements[elem]);
    cost += result;
    return result;
}

template <typename Resource, typename Cluster>
vector<vector<int> > Clusterization<Resource,Cluster>::getFixedClustersNewElements() const{
    vector<vector<int> > solution(nClusterWithFixedElements);
    for (int i = 0; i < nClusterWithFixedElements; i++){
        solution[i] = clusters[i].getNewElements();
    }
    return solution;
}

template <typename Resource, typename Cluster>
vector<vector<int> > Clusterization<Resource,Cluster>::getNewClustersElements() const{
    vector<vector<int> > solution(nK - nClusterWithFixedElements);
    for (int i = 0; i < nK - nClusterWithFixedElements; i++){
        solution[i] = clusters[i+ nClusterWithFixedElements].getNewElements();
    }
    return solution;
}

template <typename Resource, typename Cluster>
vector<Resource> Clusterization<Resource,Cluster>::getFixedClustersMeet(){
    vector<Resource> result(nClusterWithFixedElements );
    for (int i = 0; i < nClusterWithFixedElements; i++) {
        result[i] = clusters[i ].getMeet();
    }
    return result;
}

template <typename Resource, typename Cluster>
vector<Resource> Clusterization<Resource,Cluster>::getNewClustersMeet(){
    vector<Resource> result(nK - nClusterWithFixedElements );
    for (int i = 0; i < nK -nClusterWithFixedElements; i++) {
        result[i] = clusters[i + nClusterWithFixedElements].getMeet();
    }
    return result;
}

template <typename Resource, typename Cluster>
vector<Resource> Clusterization<Resource,Cluster>::getFixedClustersJoin(){
    vector<Resource> result(nClusterWithFixedElements );
    for (int i = 0; i < nClusterWithFixedElements; i++) {
        result[i] = clusters[i ].getJoin();
    }
    return result;
}

template <typename Resource, typename Cluster>
vector<Resource> Clusterization<Resource,Cluster>::getNewClustersJoin(){
    vector<Resource> result(nK - nClusterWithFixedElements );
    for (int i = 0; i < nK -nClusterWithFixedElements; i++) {
        result[i] = clusters[i + nClusterWithFixedElements].getJoin();
    }
    return result;
}


//-----------------------------------------------------------------------------------------
// A/ HEURISTICS
//-----------------------------------------------------------------------------------------
//--------------------------------
// A1/ KMEAN ALGORITHM
//--------------------------------
template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::kMeanUpdate(){
    initializeClustersToSolution();
//    for (int i = 0; i<nK; i++) {
//#ifdef DIST_MEET_JOIN
//        clusters[i].computeMeet();
//        clusters[i].computeJoin();
//#elif defined DIST_SUM_MEET
//        clusters[i].computeMeet();
//#elif defined DIST_SUM_JOIN
//        clusters[i].computeJoin();
//#else
//        throw "An objective must be defined";
//#endif
//        clusters[i].computeCost();
//    }
//    calculateCost();
}



template <typename Resource, typename Cluster>
double Clusterization<Resource,Cluster>::kMeanDistToCluster(int e, int c){ // e = initial elemnt index, c = cluster
    double dist;
    Resource meet = clusters[c].getMeet();
    Resource newMeet = meet;
    newMeet.meet(*initialElements[e]);
    dist = (clusters[c].size_allElements() +1)* meet.distance(newMeet);
    newMeet = meet;
    newMeet.join(*initialElements[e]);
    dist += newMeet.distance(meet);
    //cout << e << " " << c << " " << dist << endl;
    return dist;
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::kMeanAssign(bool & hasBeenUpdtated){
    hasBeenUpdtated = false;
    vector<bool> hasClusterBeenUpdated(nK,false);
    //vector<int> newClusterOfInitial(nInitial,-1);
    for (int e = 0; e<nInitial; e++) {
        double newClusterOfElement = -1;
        double distanceToNewCluster = INFINITY;

        for (int c = 0; c< nK; c++) {
            double dist = kMeanDistToCluster(e, c);
            if (dist < distanceToNewCluster) {
                newClusterOfElement = c;
                distanceToNewCluster = dist;
            }
        }
        if (newClusterOfElement != clusterOfInitial[e]) {
            //cout << newClusterOfElement << " " << clusterOfInitial[e] << endl;
             hasBeenUpdtated = true;
            hasClusterBeenUpdated[clusterOfInitial[e]] = true;
            clusterOfInitial[e] = newClusterOfElement;
            hasClusterBeenUpdated[clusterOfInitial[e]] = true;
        }
    }
    
    setNonAffectedClusters();
//    for (int k = 0; k<nK; k++) {
//        if (hasClusterBeenUpdated[k]) clusters[k].clearWithoutUpdatingCost();
//    }
//    
//    for (int e = 0; e<nInitial; e++) {
//        if (hasClusterBeenUpdated[clusterOfInitial[e]]) {
//            clusters[clusterOfInitial[e]].insertWithoutUpdatingCost(e,initialElements[e]);
//        }
//    }

}


template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::kMean(){
    bool hasBeenUpdated = true;
    int compt = 0;
    vector<int> bestSolution = clusterOfInitial;
    calculateCost();
    double bestCost = cost;
    while (hasBeenUpdated && compt < 100){
        compt ++;
        cout << compt << " " << cost << endl;
        kMeanAssign(hasBeenUpdated);
        kMeanUpdate();
        if (cost < bestCost){
            bestCost = cost;
            cout << " " << bestCost;
            bestSolution = clusterOfInitial;
            compt = 0;
        }
    }
    
    clusterOfInitial = bestSolution;
    initializeClustersToSolution();
    
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::kMean(double maxTime_s){
    kMean(clock() + CLOCKS_PER_SEC*maxTime_s);
    
}

template <typename Resource, typename Cluster>
void Clusterization<Resource,Cluster>::kMean(clock_t tFinal){
    bool hasBeenUpdated = true;
    int compt = 0;
    vector<int> bestSolution = clusterOfInitial;
    calculateCost();
    double bestCost = cost;
    while (hasBeenUpdated && clock() < tFinal && compt < 100){
        compt++;
        kMeanAssign(hasBeenUpdated);
        kMeanUpdate();
        if (cost < bestCost){
            bestCost = cost;
            bestSolution = clusterOfInitial;
            compt = 0;
        }
    }
    clusterOfInitial = bestSolution;
    initializeClustersToSolution();
}

//--------------------------------
// A2/ SINGLE LINK AND COMPLETE LINK ALGORITHM
//--------------------------------

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::linkAlgorithmOnResourceDistance() {
    vector<vector<double>> distanceMatrix(nInitial,vector<double>(nInitial,0));
    for (int e1 = 0; e1< nInitial-1; e1++) {
        for (int e2 = 0; e2<nInitial; e2++) {
            distanceMatrix[e1][e2] = initialElements[e1]->distance(*initialElements[e2]);
            distanceMatrix[e2][e1] = distanceMatrix[e1][e2] ;
        }
    }
    singleLinkAlgorithm(distanceMatrix);
    calculateCost();
}

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::singleLinkAlgorithm(const vector<vector<double> > &initialElementsDistance){
    int maxClusterSize = ceil(MAX_CLUSTER_RECOMBINATION_SIZE * nInitial/(float) nK) + 1;
    // Ordering of the arcs
    multimap<double,int> orderedEdges;
    for (int i = 0; i<nInitial-1; i++) {
        for (int j=i+1; j<nInitial; j++) {
            double ran = (rand()%10000)/(double) 10000; // enables to make choose randomly among the arc of same cost
            orderedEdges.insert(pair<double, int>(initialElementsDistance[i][j] + ran,i*nInitial + j));
        }
    }
    // Selection of the arcs defining the clusters (store for each vertex (initial distribution) its selected neigbors : starting from nInitial clusters, it connects the clusters using the "biggest cost" arcs until there is nK clusters axactly
    int comptConnexComponents = nInitial;
    vector<list<int>>clusterComposition(nInitial);
    clusterOfInitial = vector<int> (nInitial,-1);
    for (int i=0; i<nInitial; i++) {
        clusterComposition[i].push_back(i);
        clusterOfInitial[i] = i;
    }
    
    
    multimap<double, int>::iterator it = orderedEdges.begin();
    while (comptConnexComponents > nK && it != orderedEdges.end()) {
        ++it;
        int s1 = it->second / nInitial;
        int s2 = it->second % nInitial;
        if ((clusterOfInitial[s1] != clusterOfInitial[s2]) && (clusterComposition[clusterOfInitial[s1]].size() + clusterComposition[clusterOfInitial[s2]].size() < maxClusterSize)) { // if the clusters are different connect the two clusters
            if (clusterOfInitial[s1] > clusterOfInitial[s2]) {
                int a = s1;
                s1 =s2;
                s2 = a;
            }
            comptConnexComponents--;
            int cluster2 = clusterOfInitial[s2];
            for(list<int>::iterator clIt = clusterComposition[cluster2].begin(); clIt != clusterComposition[cluster2].end(); ++clIt) {
                clusterOfInitial[*clIt] = clusterOfInitial[s1];
            }
            clusterComposition[clusterOfInitial[s1]].merge(clusterComposition[cluster2]); // !!!! Verifier que merge est à cout fixe
            clusterComposition[cluster2] = list<int>(0);
        }
    }
    
    
    // Storing the solutions of the cluster
    // Find the index of the clusters (not necessarly the good ones) and change them
    int comptCluster = 0;
    for (int i =0; i<nInitial; i++) {
        if (clusterOfInitial[i] == i) {
            clusterOfInitial[i] = comptCluster;
            comptCluster++;
        }
        else clusterOfInitial[i] = clusterOfInitial[clusterOfInitial[i]];
    }
    setNonAffectedClusters();
    initializeClustersToSolution();
}


//-----------------------------------------------------------------------------------------
// C/ META-HEURISTICS
//-----------------------------------------------------------------------------------------
//-------------------------------
// C1/ SIMULATED ANNEALING
//-------------------------------

//---------------------
// Simulated Annealing
//---------------------

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::simulatedAnnealing(double tMax_s){
    simulatedAnnealing(tMax_s,  simulatedAnnealinginitialTemperature);
}

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::simulatedAnnealing(double tMax_s, double initialTemperature){
    if (initialTemperature < 0) {
        initialTemperature = 2 * cost;
    }
    // Time management
    clock_t beginTime = clock();
    clock_t finalTime = beginTime + CLOCKS_PER_SEC*tMax_s;
    
    // Keep in mind best solution
    double bestSolutionCost = cost;
    vector<int> bestClusterOfInitial = clusterOfInitial;
    
    // Boucle While
    while (clock() < finalTime) {
        double Temperature = initialTemperature * (finalTime - clock())/(finalTime - beginTime);
        randomNeighbor(Temperature);
//        calculateCost();
        if (cost < bestSolutionCost) {
            bestSolutionCost = cost;
            bestClusterOfInitial = clusterOfInitial;
        }
    }
    
    clusterOfInitial = bestClusterOfInitial;
    initializeClustersToSolution();
    
}

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::simulatedAnnealing(double period_s, double minImproveOnPeriod_percent, double tMax_s){
    double initialValue = cost;
    clock_t final_t = clock() + tMax_s * CLOCKS_PER_SEC;
    simulatedAnnealing(period_s, minImproveOnPeriod_percent, final_t, simulatedAnnealinginitialTemperature,initialValue);
}

template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::simulatedAnnealing(double period_s, double minImproveOnPeriod_percent, clock_t final_t, double initialTemperature, double initialValue){
    auto logger = latticeRCSP::Console::getPtrLogger();
    logger->debug("Simulated annealing: clustering {} elements into {} clusters", nInitial, nK);

	if (clusters.size() < 2 || nInitial < 2) return;
    
    // Time management
    clock_t beginTime = clock();
    
    // Keep in mind best solution
    double bestSolutionCost = cost;
    vector<int> bestClusterOfInitial = clusterOfInitial;
    
    // Continue only if improved during the last period
    bool stop = false;
    unsigned int countIterations = 0;
    while ((clock() < final_t) && (!stop)) {
    	++countIterations;
        clock_t periodBeginTime = clock();
        clock_t periodEndTime = periodBeginTime + CLOCKS_PER_SEC * period_s;
        double previousSolutionCost = bestSolutionCost;
        
        // SimulatedAnnealing for one period
        while (clock() < periodEndTime) {
            double Temperature = initialTemperature * (final_t - clock())/(final_t - beginTime);
            randomNeighbor(Temperature);
            if (cost < bestSolutionCost) {
                bestSolutionCost = cost;
                bestClusterOfInitial = clusterOfInitial;
            }
        }
        
        // test improve
        if (100 * (previousSolutionCost - bestSolutionCost)/(initialValue - bestSolutionCost) < minImproveOnPeriod_percent) {
            stop = true;
        }
    }
    
    clusterOfInitial = bestClusterOfInitial;
    initializeClustersToSolution();
    logger->debug("Simulated annealing iterations: {}", countIterations);
}


template <typename Resource,typename Cluster>
void Clusterization<Resource,Cluster>::randomNeighbor(double Temperature){
    if (rand()%2 == 0){
        // swap
        int firstIndice = rand()%nInitial;
        int secondIndice = rand()%nInitial;
        while (secondIndice == firstIndice) {
            secondIndice = rand()%nInitial;
        }
        
        double Delta = swap(firstIndice,secondIndice);
        if (Delta > 0) {
            double test = rand()% 10000;
            test = test/10000;
            double boltzmann = exp(- Delta/Temperature);
            if (test > boltzmann){
                swap(firstIndice, secondIndice);
            } 
        }
    }
    else {
        // add and delete
        int indice = rand()%nInitial;
        while (clusters[clusterOfInitial[indice]].size_allElements()<2) {
            indice = rand()%nInitial;
        }
        int cluster = rand()%nK;
        
        int initialCluster = clusterOfInitial[indice];
        
        double Delta = addRemove(indice, cluster);
        if (Delta > 0) {
            double test = rand()% 10000;
            test = test/10000;
            double boltzmann = exp(- Delta/Temperature);
            if (test > boltzmann){
                addRemove(indice, initialCluster);
            }
        }
    }
}

