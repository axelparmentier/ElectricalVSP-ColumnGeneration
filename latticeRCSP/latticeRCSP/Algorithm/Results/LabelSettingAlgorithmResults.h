//
//  LabelSettingAlgorithmResults.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__LabelSettingAlgorithmResults__
#define __latticeRCSP_v2__LabelSettingAlgorithmResults__

#include <stdio.h>
#include <ctime>
#include "../../Tools/ResultsPrinter.h"

using namespace ToolsAxel;
namespace latticeRCSP {
    
    class LabelSettingAlgorithmResults : public AlgorithmResults {
    public:
        bool isSolvedToOptimal;
        bool newSolutionsFound;
        
        double lowerBound;
        double upperBound;
        
        int nLabelsTreated;
        int nLabelsExpanded;
        clock_t executionTime;

//    Used only by LabelSetting
        int nSettingCuts;
        
//        Used only by LabelCorrectingAlgorithm
        int nBoundCut;
        int nDominanceCut;
        int nNonDominated;

    public:
        LabelSettingAlgorithmResults();
        void initialize();
        
        void printResults_treatedTime(ofstream &);
    };
}

#endif /* defined(__latticeRCSP_v2__LabelSettingAlgorithmResults__) */
