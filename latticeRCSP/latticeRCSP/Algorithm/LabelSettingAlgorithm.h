//
//  LabelSettingAlgorithm.h
//  RCSP
//
//  Created by Axel Parmentier on 14/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__LabelSettingAlgorithm__
#define __RCSP__LabelSettingAlgorithm__

#define LABEL_SETTING_KEY_PARTIAL_PATH_MINUS_MULTIPLE 0.1

#include <iostream>
#include <fstream>

#include "../Graph/Path.h"
#include "../Graph/PathSet.h"


#include "../Tools/ResultsPrinter.h"

#include "Results/LabelSettingAlgorithmResults.h"

//-------------------------
// Choice of the keyFunctors
//-------------------------
// There are three types of keyFunctors
//  - The key functors of the (eventual) conditional graph: it is contained in typename boundSetType
//  - The key functor that gives the cost minimized by the algorithm
//  - The key functor that gives the constraint
//-------------------------
 
    
//-------------------------------------------------------------
// Label setting algorithm class
//-------------------------------------------------------------
using namespace ToolsAxel;
namespace latticeRCSP {
   
template <typename Resource, typename BoundedGraph>
class LabelSettingAlgorithm {
protected:
    BoundedGraph * graph;
    
    // Algorithm structures
    double const * costOfWorstSolutionInSolutionSet;
    multimap<double, Path<Resource > > labelMap;
    LabelSettingAlgorithmResults results;
    
    // Statistic structures
    double initialCost;
    int initialSize;
    
    clock_t t0;
    int nLabelsTreated;
    int nLabelsExpanded;
    int nLabelsCut; // Only For Label Setting, not for label correcting
    clock_t t1;
    
    bool stop;
    bool stoppedBeforeEnd;
    bool newSolutionFound;
    
    // Max time monitoring structure -------------
    clock_t maxDuration;
    int numberOfIterationsBetweenTwoTests;
    double minImprovePercent;
    int maxNumberOfIterations;
    
    clock_t finalTime;
    double costOfWorstSolutionAtLastTest;
    // ---------------------------------------------
    // Max size monitoring structure -------------
    unsigned int labelMapMaxSize;
    // ---------------------------------------------
    
    //-------------
    // Approx res
    //-------------
    bool isStopBoundActivated_m;
    double stopBound_m;

    //-------------
    // MemoryBounded
    //-------------
    bool isMemoryBounded;
    bool isMemoryBoundedExact;
    int memoryBoundedMaxSize;

public:
    LabelSettingAlgorithm(BoundedGraph * );
    
    //------------------------------------
    // Getters and Setters
    //------------------------------------
    LabelSettingAlgorithmResults getResults() const;
    bool getIsSolvedToOptimum();
    bool getHasNewSolutionsBeenFound();
    void setLabelMapMaxSize(unsigned int);

    //StopBound
    void setIsStopBoundActivated(bool isStopBoundActivated){isStopBoundActivated_m = isStopBoundActivated;}
    void setStopBound(double stopBound){stopBound_m = stopBound;}

    //MemoryBounded
    void setIsMemoryBounded(bool b){isMemoryBounded = b;}
    void setMemoryBoundedMaxSize(int s){memoryBoundedMaxSize = s;}
    bool getIsMemoryBoundedExact() const {return isMemoryBoundedExact;}

    
    //------------------------------------
    // Solver
    //------------------------------------
    void solve();
protected:
    // Solve() dependendencies -- non modified by inherited classes
    void initialPrints();
    void initializeReverseBoundsIfNotInitialized();
    void initializeStatisticStructures();
    void initializeLabelMapWithOrigin();
    void midAlgorithmPrints();
    void expandCurrentPath();
    void updateMinimumKnownCostAndLabelMap();
    void finalPrintsAndTests_shared();
    void finalPrintsAndTests();
    void updateProblemIfNewLowerBoundsWereFound();
    void memoryBoundedClean();
    
    // Solve() dependencies -- modified by inherited classes
    void initializeAlgorithmStoppingStructures();
    void testIfMustBeStoppedBeforeEnd();
    
    void initializeAlgorithmStoppingStructures_withMaxTimeMonitoring();
    void testIfMustBeStoppedBeforeEnd_withMaxTimeMonitoring();
    
    void testIfMustBeStoppedBeforeEnd_withLabelMapMaxSize();
    
};
    
//-------------------------------------------------------------
// Inherited classe to stop algorithm at a given time
//-------------------------------------------------------------
    
    template <typename Resource, typename BoundedGraph>
    class LabelSettingAlgorithm_withMaxTimeMonitoring : public LabelSettingAlgorithm<Resource, BoundedGraph>{
    public:
        LabelSettingAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration, int numberOfIterationsBetweenTwoTests, double minImprovePercent, int maxNumberOfIterations = INFINITY);
        
        void solve();
        
    };

//-------------------------------------------------------------
// Inherited classe to stop algorithm if label Map becomes to large
//-------------------------------------------------------------
    
    template <typename Resource, typename BoundedGraph>
    class LabelSettingAlgorithm_withLabelMapMaxSize : public LabelSettingAlgorithm<Resource, BoundedGraph>{
    public:
        LabelSettingAlgorithm_withLabelMapMaxSize(BoundedGraph * graph,unsigned int maxSize);
        
        void solve();
//    protected:
//        void testIfMustBeStoppedBeforeEnd();
        
    };
}
#include "LabelSettingAlgorithm.hpp"

#endif /* defined(__RCSP__LabelSettingAlgorithm__) */
