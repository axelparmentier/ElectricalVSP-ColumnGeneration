//
//  latticeClusterization.h
//  latticeClustering
//
//  Created by Axel Parmentier on 09/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeClustering__latticeClusterization__
#define __latticeClustering__latticeClusterization__

#include <stdio.h>
#include <vector>
#include <math.h>
#include <cstdlib>

#define SIMULATED_ANNEALING_INITIAL_TEMPERATURE 1e-3

// Recombinaison from local solution parameter
#define MAX_CLUSTER_RECOMBINATION_SIZE 1.3 // the maximum cluster size in the recombination process is MAX_CLUSTER_RECOMBINATION_SIZE * nInitial/nK
#define LINK_ALG_SINGLE_LINK 0
#define LINK_ALG_COMPLETE_LINK 1

using namespace std;

template <typename Resource, typename Cluster>
class Clusterization {
    // Fixed clusters and non fixed clusters
    int nClusterWithFixedElements;
    vector<int> inputIndexOfElement;
    
    //------------------------
    int nInitial;
    int nK;
    
    vector<const Resource *> initialElements;
    vector<int> clusterOfInitial;
    vector<Cluster> clusters;
    
    double cost;
public:
    typedef enum {
        SUM_OF_CLUSTERS_COST,
        MAX_OF_CLUSTERS_COST
    } CostPolicy;
    
private:
    static CostPolicy costPolicy_s;
    
public:
    //--------------------------------
    // Constructor
    //--------------------------------
    Clusterization(int nK,vector<const Resource *> const &);
    Clusterization(int nNew, vector<vector<Resource const *> > const &, vector<Resource const *> const &);

    static void setCostPolicy(CostPolicy);
protected:
    //--------------------------------
    // Cluster initialization
    //--------------------------------
    void setRandomSolution(); // build randomly the clusters
    void setNonAffectedClusters();
    
    void initializeClustersToSolution();
    void initializeFixedCluster(vector<vector<Resource const *> > const &);

    //--------------------------------
    // Cost
    //--------------------------------
    void calculateCost();
    
    //--------------------------------
    // Neighborhood
    //--------------------------------
    double swap(int firstElement,int secondElement);
    double addRemove(int elem,int newCluster);
    
    //--------------------------------
    // Getters and setters
    //--------------------------------
public:
    vector<vector<int> > getFixedClustersNewElements() const;
    vector<vector<int> > getNewClustersElements() const;
    
    vector<Resource> getFixedClustersMeet();
    vector<Resource> getNewClustersMeet();

    vector<Resource> getFixedClustersJoin();
    vector<Resource> getNewClustersJoin();

    //-----------------------------------------------------------------------------------------
    // A/ HEURISTICS
    //-----------------------------------------------------------------------------------------
    //--------------------------------
    // A1/ KMEAN ALGORITHM
    //--------------------------------
protected:
    void kMeanUpdate();
    
    void kMeanAssign(bool & hasBeenUpdated);
    double kMeanDistToCluster(int initial, int cluster);
    
public:
    void kMean();
    void kMean(double maxTime_s);
    void kMean(clock_t final_t);
    
    //--------------------------------
    // A2/ SINGLE LINK AND COMPLETE LINK ALGORITHM
    //--------------------------------
    void linkAlgorithmOnResourceDistance();
protected:
    void singleLinkAlgorithm(const vector<vector<double>> &cardinalEventSameCluster); // Agglomerative single link 
    
    //-----------------------------------------------------------------------------------------
    // C/ META-HEURISTICS
    //-----------------------------------------------------------------------------------------
    //-------------------------------
    // C1/ SIMULATED ANNEALING
    //-------------------------------
    //---------------------
    // Simulated Annealing
    //---------------------
    double simulatedAnnealinginitialTemperature = SIMULATED_ANNEALING_INITIAL_TEMPERATURE;
    void simulatedAnnealing(double tMax_s,double initialTemperature);
    void simulatedAnnealing(double period_s, double minImproveOnPeriod_percent, clock_t final_t, double initialTemperature, double initialValue);
    
public:
    void simulatedAnnealing(double tMax_s);
    void simulatedAnnealing(double period_s, double minImproveOnPeriod_percent, double tMax_s);
    
protected:
    void randomNeighbor(double Temperature);
    
    
    //-------------------------------
    // C2/ WEIGHTED TABOO
    //-------------------------------
//protected:
//    //--------------------
//    // Taboo Variables
//    //--------------------
//    // parameters
//    int tabooNSwapForOneAddRemove = TABOO_N_SWAP_FOR_1_ADDREMOVE;
//    int tabooNSamplesToFindNeighboor = TABOO_N_SAMPLE_TO_FIND_NEIGHBOOR;
//    bool tabooInitialFinalKMean = TABOO_WITH_INITIAL_FINAL_KMEAN;
//    bool tabooInsidekMean = TABOO_WITH_KMEAN;
//    
//    // taboo
//    int tabooListActualLength = 0;
//    int tabooListMaxLength = TABOO_LIST_MAX_LENGTH;
//    bool isTabooActivated = true;
//    vector<vector<bool>> forbiddenClusterIK;
//    list<pair<int, int>> tabooListForbiddenClusterIK;
//    
//    // Weights
//    vector<vector<double>> tabooRelationMatrix;
//    vector<vector<double>> tabooSummedRelationMatrix;
//    vector<double> tabooLinSum;
//    //vector<double> tabooColSum;
//    
//    // best Solution
//    vector<int> tabooBestClusterOfInitial;
//    double tabooBestCost;
//    
//    //--------------------
//    // Weighted Neighboorhood
//    //--------------------
//    void weightedAddRemove(int & solInitialDist, int & solCluster, double & solCost, bool tabooActivated); // only search a potentially interesting add Remove and calculate the new cost (but let in the initial distribution
//    void weightedSwap(int & solDist1, int & solDist2, double & solCost, bool tabooActivated);
//    
//    //--------------------
//    // Weighted Taboo
//    //--------------------
//    void weightedTabooInitialization(int nSamples, int nMixtures, double maxTimeKMean_s = 0.1);
//    void addClusterSolutionToTabooRelationMatrix(vector<int> &univClusterOfInitial, double solCost);
//    void addToClusterTaboo(int newElement, list<int> &clusterElements, double solCost);
//    void updateTabooSummedRelationMatrix();
//    
//    void oneStepTaboo(int nSample);
//    void updateTabooList(int initial, int cluster);
//    
//    void weightedTaboo(double periodOfTimeForImprove_s, double minImproveDuringPeriod_percent, clock_t final_t, double initialValue);
//    
//public:
//    void weightedTaboo(double maxTime_s);
//    void weightedTaboo(double periodOfTimeForImprove_s, double minImproveDuringPeriod_percent, double maxTime_s);
    
    
    
};

#include "Clusterization.hpp"

#endif /* defined(__latticeClustering__latticeClusterization__) */
