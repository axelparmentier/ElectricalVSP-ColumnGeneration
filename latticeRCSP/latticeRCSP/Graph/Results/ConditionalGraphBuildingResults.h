//
//  ConditionalGraphBuildingResults.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ConditionalGraphBuildingResults__
#define __latticeRCSP_v2__ConditionalGraphBuildingResults__

#include <stdio.h>
#include "../../Tools/ResultsPrinter.h"
#include "GraphWithBoundsResults.h"

using namespace ToolsAxel;

namespace latticeRCSP{
    class ConditionalGraphBuildingResults : public AlgorithmResults {
    public:
        double maxDifference;
        int upperBoundNumberOfStates;
        
        int nStateVertices;
        int nStateArcs;
        double executionBuildingTime;
        
        int nVerticesWithStates;
        int nStateVerticesReachable_withIntraVertexStateArcs;
        int nStateVerticesReachable_withoutIntraVertexStateArcs;
        
        MeetAlgorithmResults meetAlgorithm_withIntraVertexStateArcs;
        DijkstraLikeHeuristicResults dijsktraHeuristicResults;
        
        MeetAlgorithmResults meetAlgorithm_withoutIntraVertexStateArcs;
        
    public:
        ConditionalGraphBuildingResults();
        void initialize();
        
        //--------------------------
        // Results printing functions for Axel's article
        //--------------------------
        void printResults_nonRecursive(ofstream &);
    };
}

#endif /* defined(__latticeRCSP_v2__ConditionalGraphBuildingResults__) */
