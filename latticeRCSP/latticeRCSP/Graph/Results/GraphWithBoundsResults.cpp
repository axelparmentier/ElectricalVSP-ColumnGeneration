//
//  GraphWithBoundsResults.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "GraphWithBoundsResults.h"

namespace latticeRCSP {
    //--------------------------
    // Results
    //--------------------------
    
    MeetAlgorithmResults::MeetAlgorithmResults(){
        initialize();
    }
    
    void MeetAlgorithmResults::initialize(){
        isAcyclic = false;
        nbUpdates = 0;
        nbExpansions = 0;
        nbVerticesReachable = 0;
        executionTime = 0;
        
    }
    
    //--------------------------
    // Results printing functions for Axel's article
    //--------------------------
    
    void MeetAlgorithmResults::printResults_gammaTime(ofstream &write){
        AlgorithmResults::printSeparatorIfNotAtTheBeginningOfALine(write);
        write.precision(1);
        write << nbExpansions /(double) nbVerticesReachable << AlgorithmResults::separator;
        write.precision(2);
        write << executionTime /(double) CLOCKS_PER_SEC;
        
    }
    
    DijkstraLikeHeuristicResults::DijkstraLikeHeuristicResults(){
        initialize();
    }
    
    void DijkstraLikeHeuristicResults::initialize(){
        isAcyclic = false;
        nbVerticesWithUpperBound = 0;
        executionTime = 0;
    }
}