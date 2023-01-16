//
//  GraphWithBoundsResults.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__GraphWithBoundsResults__
#define __latticeRCSP_v2__GraphWithBoundsResults__

#include <cstdio>
#include <ctime>
#include "../../Tools/ResultsPrinter.h"

using namespace ToolsAxel;
namespace latticeRCSP {
    
    class MeetAlgorithmResults : public AlgorithmResults {
    public:
        bool isAcyclic;
        int nbUpdates;
        int nbExpansions;
        int nbVerticesReachable;
        clock_t executionTime;
        
    public:
        MeetAlgorithmResults();
        void initialize();
        
        //--------------------------
        // Results printing functions for Axel's article
        //--------------------------
        void printResults_gammaTime(ofstream & write);
    };
    
    class DijkstraLikeHeuristicResults{
    public:
        bool isAcyclic;
        int nbVerticesWithUpperBound;
        clock_t executionTime;
        
    public:
        DijkstraLikeHeuristicResults();
        void initialize();
    };
}

#endif /* defined(__latticeRCSP_v2__GraphWithBoundsResults__) */
