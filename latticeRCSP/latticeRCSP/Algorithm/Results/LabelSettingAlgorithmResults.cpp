//
//  LabelSettingAlgorithmResults.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "LabelSettingAlgorithmResults.h"

namespace latticeRCSP{
    LabelSettingAlgorithmResults::LabelSettingAlgorithmResults() {
        initialize();
    }
    
    void LabelSettingAlgorithmResults::initialize(){
        isSolvedToOptimal = false;
        nLabelsExpanded = 0;
        nLabelsTreated = 0;
        executionTime = 0;
        lowerBound = -INFINITY;
        upperBound = +INFINITY;
        nSettingCuts = 0;
        nBoundCut = 0;
        nDominanceCut = 0;
        nNonDominated = 0;
    }
    
    void LabelSettingAlgorithmResults::printResults_treatedTime(ofstream & write){
        AlgorithmResults::printSeparatorIfNotAtTheBeginningOfALine(write);
        write << nLabelsTreated << AlgorithmResults::separator;
        write << nLabelsExpanded << AlgorithmResults::separator;
        write.precision(2);
        write << executionTime /(double) CLOCKS_PER_SEC << AlgorithmResults::separator;
        if (isSolvedToOptimal){
            write << "0" ;
        } else{
            write << (upperBound - lowerBound) / lowerBound ;
        }
    }
}
