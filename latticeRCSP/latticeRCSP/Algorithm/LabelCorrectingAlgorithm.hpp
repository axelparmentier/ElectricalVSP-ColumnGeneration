//
//  labelCorrectingWithKey.cpp
//  RCSP
//
//  Created by Axel Parmentier on 19/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "LabelCorrectingAlgorithm.h"

namespace latticeRCSP{
    
    template <typename Resource, typename BoundedGraph>
    LabelCorrectingAlgorithm<Resource, BoundedGraph>::LabelCorrectingAlgorithm(BoundedGraph * graph) : LabelSettingAlgorithm<Resource, BoundedGraph>(graph){
       nNonDominated = 0;
       nBoundsCut = 0;
       nDominanceCut = 0;
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm<Resource, BoundedGraph>::solve(){
        vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());
        
        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
        initialPrints();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeReverseBoundsIfNotInitialized();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeStatisticStructures();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeAlgorithmStoppingStructures();
        
        //---------------------------------------------------
        // Algorithm
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t1 = clock();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeLabelMapWithOrigin();
        
        LabelSettingAlgorithm<Resource,BoundedGraph>::stop = false; // Identify if the algorithm must be stopped (optimal path are already generated)
        while (!LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.empty() && !LabelSettingAlgorithm<Resource,BoundedGraph>::stop) {
            //---------------------------------------------------
            // Stopping before end tests
            //---------------------------------------------------
            LabelSettingAlgorithm<Resource,BoundedGraph>::testIfMustBeStoppedBeforeEnd();
            LabelSettingAlgorithm<Resource,BoundedGraph>::midAlgorithmPrints();
            
            //---------------------------------------------------
            // Expansion
            //---------------------------------------------------
            expandCurrentPath();
            LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();

            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            updateMinimumKnownCostAndLabelMap();
            
        }
        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        finalPrintsAndTests();
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm<Resource, BoundedGraph>::updateMinimumKnownCostAndLabelMap(){
		if (LabelSettingAlgorithm<Resource, BoundedGraph>::labelMap.empty()) return;
        if (LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.begin()->first > *LabelSettingAlgorithm<Resource,BoundedGraph>::costOfWorstSolutionInSolutionSet + EPS) {
           LabelSettingAlgorithm<Resource,BoundedGraph>::stop = true;
        }
        size_t s1 = LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.size();
        LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.erase(LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.upper_bound(*LabelSettingAlgorithm<Resource,BoundedGraph>::costOfWorstSolutionInSolutionSet + EPS), LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.end());
        size_t s2 = LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.size();
        nBoundsCut += s1 - s2;
    }

    template <typename Resource, typename BoundedGraph>
        void LabelCorrectingAlgorithm<Resource, BoundedGraph>::initialPrints(){
    	auto logger = Console::getPtrLogger();
    	logger->info("--------------------------     Label correcting Algorithm      --------------------------");
#ifdef KEY_WITHOUT_BOUND
       cout << endl << "Keys without bounds: c(q_p)";
#endif
       logger->debug("Nota bene: problem solution set is not emptied at the beginning of the algorithm");
       nNonDominated = 0;
       nBoundsCut = 0;
       nDominanceCut = 0;
    }


    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm<Resource, BoundedGraph>::expandCurrentPath(){
        LabelSettingAlgorithm<Resource,BoundedGraph>::nLabelsExpanded++;
        
        typename multimap<double, Path<Resource > >::iterator currentIt= LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.begin();
        Path<Resource> currentPath = currentIt->second;
        LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.erase(currentIt);
        
        int currentVertex = currentPath.getDestination();
        
        // -------------------------------------------------------------------------------
        // Additional test for label correcting -----------------------------------------
        if (vertexNonDominatedPathSet[currentVertex].insertIfNonDominated(currentPath, nNonDominated)) {
        // ------------------------------------------------------------------------------
            if (LabelSettingAlgorithm<Resource,BoundedGraph>::graph->vertexType(currentVertex) != -1){// If it is not a destination
                for (int a = 0; a < LabelSettingAlgorithm<Resource,BoundedGraph>::graph->deltaPa(currentVertex).size(); a++) {
                    LabelSettingAlgorithm<Resource,BoundedGraph>::nLabelsTreated++;
                    int arc = LabelSettingAlgorithm<Resource,BoundedGraph>::graph->deltaPa(currentVertex)[a];
                    Path<Resource> newPath = currentPath;
                    newPath.expand(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getArc(arc));
                    
                    double lowerBoundOnCostFromLabel = INFINITY;
                    
                    if (!LabelSettingAlgorithm<Resource,BoundedGraph>::graph->testCompleteDominatedAndUpdateProblemSolution(newPath, lowerBoundOnCostFromLabel)){
                        LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.insert(pair<double,Path<Resource> >(lowerBoundOnCostFromLabel, newPath));
                    } else {
                       ++LabelSettingAlgorithm<Resource,BoundedGraph>::nLabelsCut;
                       nBoundsCut++;
                    }
                }
            }
        } else {
           ++nDominanceCut;
           ++LabelSettingAlgorithm<Resource,BoundedGraph>::nLabelsCut;
        }
        
        
    }

    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests(){
//       cout<< "Label correcting algorihtm Performance: ";
       LabelSettingAlgorithm<Resource, BoundedGraph>::results.nBoundCut = nBoundsCut;
       LabelSettingAlgorithm<Resource, BoundedGraph>::results.nDominanceCut = nDominanceCut;
       LabelSettingAlgorithm<Resource, BoundedGraph>::results.nNonDominated = nNonDominated;
       LabelSettingAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests_shared();
    }

    //----------------------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    LabelCorrectingAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::LabelCorrectingAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration0, int numberOfIterationsBetweenTwoTests0, double minImprovePercent0, int maxNumberOfIterations0) : LabelCorrectingAlgorithm<Resource, BoundedGraph>(graph){
        LabelSettingAlgorithm<Resource,BoundedGraph>::maxDuration = maxDuration0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::numberOfIterationsBetweenTwoTests = numberOfIterationsBetweenTwoTests0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::minImprovePercent = minImprovePercent0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::maxNumberOfIterations = maxNumberOfIterations0;
        
    }
    
    
    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::solve(){
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());

        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::initialPrints();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeReverseBoundsIfNotInitialized();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeStatisticStructures();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeAlgorithmStoppingStructures_withMaxTimeMonitoring();
        
        //---------------------------------------------------
        // Algorithm
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t1 = clock();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeLabelMapWithOrigin();
        
        LabelSettingAlgorithm<Resource,BoundedGraph>::stop = false; // Identify if the algorithm must be stopped (optimal path are already generated)
        while (!LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.empty() && !LabelSettingAlgorithm<Resource,BoundedGraph>::stop) {
            //---------------------------------------------------
            // Stopping before end tests
            //---------------------------------------------------
            LabelSettingAlgorithm<Resource,BoundedGraph>::testIfMustBeStoppedBeforeEnd_withMaxTimeMonitoring();
            LabelSettingAlgorithm<Resource,BoundedGraph>::midAlgorithmPrints();
            
            //---------------------------------------------------
            // Expansion
            //---------------------------------------------------
            LabelCorrectingAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
            LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();
            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            LabelCorrectingAlgorithm<Resource, BoundedGraph>::updateMinimumKnownCostAndLabelMap();
            
        }
        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
    }
    
    
    //----------------------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    LabelCorrectingAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::LabelCorrectingAlgorithm_withLabelMapMaxSize(BoundedGraph * graph, unsigned int maxSize) : LabelCorrectingAlgorithm<Resource, BoundedGraph>(graph){
        LabelSettingAlgorithm<Resource,BoundedGraph>::labelMapMaxSize = maxSize;
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelCorrectingAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::solve(){
        
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());

        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::initialPrints();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeReverseBoundsIfNotInitialized();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeStatisticStructures();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeAlgorithmStoppingStructures();
        
        //---------------------------------------------------
        // Algorithm
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t1 = clock();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initializeLabelMapWithOrigin();
        
        LabelSettingAlgorithm<Resource,BoundedGraph>::stop = false; // Identify if the algorithm must be stopped (optimal path are already generated)
        while (!LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.empty() && !LabelSettingAlgorithm<Resource,BoundedGraph>::stop) {
            //---------------------------------------------------
            // Stopping before end tests
            //---------------------------------------------------
           if (LabelCorrectingAlgorithm<Resource,BoundedGraph>::nNonDominated > 5 * LabelSettingAlgorithm<Resource,BoundedGraph>::labelMapMaxSize){
               LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
               LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
            }
           LabelSettingAlgorithm<Resource,BoundedGraph>::testIfMustBeStoppedBeforeEnd_withLabelMapMaxSize();
            LabelSettingAlgorithm<Resource,BoundedGraph>::midAlgorithmPrints();
            
            //---------------------------------------------------
            // Expansion
            //---------------------------------------------------
            LabelCorrectingAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
            LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();
            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            LabelCorrectingAlgorithm<Resource, BoundedGraph>::updateMinimumKnownCostAndLabelMap();
            
        }
        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        LabelCorrectingAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
    }
}
