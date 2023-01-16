//
//  labelCorrectingWithKey.h
//  RCSP
//
//  Created by Axel Parmentier on 19/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__labelCorrectingWithKey__
#define __RCSP__labelCorrectingWithKey__

#include <iostream>
#include <vector>
#include "LabelSettingAlgorithm.h"
#include "../Graph/NonDominatedPathSet.h"



namespace latticeRCSP {
    template <typename Resource, typename BoundedGraph>
    class LabelCorrectingAlgorithm : public LabelSettingAlgorithm<Resource, BoundedGraph> {
    protected:
        vector<NonDominatedPathSet<Resource> > vertexNonDominatedPathSet;
        
        int nNonDominated;
        int nBoundsCut;
        int nDominanceCut;

    public:
        LabelCorrectingAlgorithm(BoundedGraph * );
        
        void solve();
        
        //---------------------
        // Redefined methods compared to label setting algorithm
        //---------------------
    protected:
        void initialPrints();
        void expandCurrentPath();
        void finalPrintsAndTests();
        void updateMinimumKnownCostAndLabelMap();

        
    };
    
    
//    ------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    class LabelCorrectingAlgorithm_withMaxTimeMonitoring : public LabelCorrectingAlgorithm<Resource, BoundedGraph> {
    protected:
        vector<NonDominatedPathSet<Resource> > vertexNonDominatedPathSet;
        
    public:
        LabelCorrectingAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration, int numberOfIterationsBetweenTwoTests, double minImprovePercent, int maxNumberOfIterations = INFINITY);;
        void solve();
    };
    
    //    ------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    class LabelCorrectingAlgorithm_withLabelMapMaxSize : public LabelCorrectingAlgorithm<Resource, BoundedGraph> {
    protected:
        vector<NonDominatedPathSet<Resource> > vertexNonDominatedPathSet;
        
    public:
        LabelCorrectingAlgorithm_withLabelMapMaxSize(BoundedGraph *, unsigned int maxSize);
        void solve();
    };
    
    
}
#include "LabelCorrectingAlgorithm.hpp"

#endif /* defined(__RCSP__labelCorrectingWithKey__) */
