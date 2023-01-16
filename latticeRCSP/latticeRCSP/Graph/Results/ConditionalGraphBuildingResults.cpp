//
//  ConditionalGraphBuildingResults.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ConditionalGraphBuildingResults.h"

namespace latticeRCSP {
    ConditionalGraphBuildingResults::ConditionalGraphBuildingResults(){
        initialize();
    }
    
    void ConditionalGraphBuildingResults::initialize(){
        maxDifference = -1;
        upperBoundNumberOfStates = -1;
        nStateArcs = -1;
        nStateVertices = -1;
        executionBuildingTime = 0;
        
        nVerticesWithStates = -1;
        nStateVerticesReachable_withIntraVertexStateArcs = -1;
        nStateVerticesReachable_withoutIntraVertexStateArcs = -1;
    }
    
    //------------------------------------------------------------------
    // Results printing functions for Axel's article
    //------------------------------------------------------------------
    
    void ConditionalGraphBuildingResults::printResults_nonRecursive(ofstream & write){
        AlgorithmResults::printSeparatorIfNotAtTheBeginningOfALine(write);
        
        write << std::fixed;
        write.precision(2);
        
        write << maxDifference << AlgorithmResults::separator;
        write << nStateVertices << AlgorithmResults::separator;
        write << executionBuildingTime /(double) CLOCKS_PER_SEC;
        
        meetAlgorithm_withIntraVertexStateArcs.printResults_gammaTime(write);
    }
}
