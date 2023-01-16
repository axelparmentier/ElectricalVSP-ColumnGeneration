/*
 * LabelDominanceAlgorithm.cpp
 *
 *  Created on: 26 juil. 2016
 *      Author: axelparmentier
 */

#include "LabelDominanceAlgorithm.h"

namespace latticeRCSP {

template <typename Resource, typename BoundedGraph>
LabelDominanceAlgorithm<Resource,BoundedGraph>::LabelDominanceAlgorithm(BoundedGraph * graph) : LabelSettingAlgorithm<Resource, BoundedGraph>(graph)
{
   nNonDominated = 0;
   nBoundsCut = 0;
   nDominanceCut = 0;
}

template <typename Resource, typename BoundedGraph>
LabelDominanceAlgorithm<Resource,BoundedGraph>::~LabelDominanceAlgorithm()
{

}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm<Resource,BoundedGraph>::initializeReverseCandidatePathsIfNotInitialized() {
   if (!LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getIsReverseCandidatePathSet()){
   	auto logger = Console::getPtrLogger();
   	logger->info("Candidate paths must be set");
    LabelSettingAlgorithm<Resource,BoundedGraph>::graph->setReverseCandidatePath();
      }
}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm<Resource,BoundedGraph>::solve(){
   vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());

   //---------------------------------------------------
   // Initial checks and prints
   //---------------------------------------------------


   LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
   initialPrints();
   initializeReverseCandidatePathsIfNotInitialized();
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
       LabelSettingAlgorithm<Resource,BoundedGraph>::updateMinimumKnownCostAndLabelMap();

   }

   //---------------------------------------------------
   // Final prints and tests
   //---------------------------------------------------
   finalPrintsAndTests();
   LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm<Resource,BoundedGraph>::initialPrints(){
	auto logger = Console::getPtrLogger();
   logger->info("--------------------------     Label dominance Algorithm       --------------------------");
   logger->debug("Nota bene: problem solution set is not emptied at the beginning of the algorithm");
   nNonDominated = 0;
   nBoundsCut = 0;
   nDominanceCut = 0;
}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm<Resource, BoundedGraph>::expandCurrentPath(){
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

                if (!LabelSettingAlgorithm<Resource,BoundedGraph>::graph->testPartialDominatedAndUpdateProblemSolution(newPath, lowerBoundOnCostFromLabel)){
                    LabelSettingAlgorithm<Resource,BoundedGraph>::labelMap.insert(pair<double,Path<Resource> >(lowerBoundOnCostFromLabel, newPath));
                } else {
//                       LabelSettingAlgorithm<Resource,BoundedGraph>::nLabelsCut++;
                   nBoundsCut++;
                }
            }
        }
    } else {
       ++nDominanceCut;
    }
}


template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests(){
//   cout<< "Label dominance algorithm Performance: ";
    LabelSettingAlgorithm<Resource, BoundedGraph>::results.nBoundCut = nBoundsCut;
    LabelSettingAlgorithm<Resource, BoundedGraph>::results.nDominanceCut = nDominanceCut;
    LabelSettingAlgorithm<Resource, BoundedGraph>::results.nNonDominated = nNonDominated;
    LabelSettingAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests_shared();
}

//----------------------------------------------------------------------------------------------------------
template <typename Resource, typename BoundedGraph>
LabelDominanceAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::LabelDominanceAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration0, int numberOfIterationsBetweenTwoTests0, double minImprovePercent0, int maxNumberOfIterations0) : LabelDominanceAlgorithm<Resource, BoundedGraph>(graph){
    LabelSettingAlgorithm<Resource,BoundedGraph>::maxDuration = maxDuration0;
    LabelSettingAlgorithm<Resource,BoundedGraph>::numberOfIterationsBetweenTwoTests = numberOfIterationsBetweenTwoTests0;
    LabelSettingAlgorithm<Resource,BoundedGraph>::minImprovePercent = minImprovePercent0;
    LabelSettingAlgorithm<Resource,BoundedGraph>::maxNumberOfIterations = maxNumberOfIterations0;

}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::solve(){
   LabelDominanceAlgorithm<Resource,BoundedGraph>::vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());

    //---------------------------------------------------
    // Initial checks and prints
    //---------------------------------------------------
    LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
    LabelDominanceAlgorithm<Resource,BoundedGraph>::initialPrints();
    LabelDominanceAlgorithm<Resource,BoundedGraph>::initializeReverseCandidatePathsIfNotInitialized();
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
        LabelDominanceAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
         LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();

        //---------------------------------------------------
        // Test if it must be stopped
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateMinimumKnownCostAndLabelMap();

    }

    //---------------------------------------------------
    // Final prints and tests
    //---------------------------------------------------
    LabelDominanceAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
    LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
}


//----------------------------------------------------------------------------------------------------------
template <typename Resource, typename BoundedGraph>
LabelDominanceAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::LabelDominanceAlgorithm_withLabelMapMaxSize(BoundedGraph * graph, unsigned int maxSize) : LabelDominanceAlgorithm<Resource, BoundedGraph>(graph){
    LabelSettingAlgorithm<Resource,BoundedGraph>::labelMapMaxSize = maxSize;
}

template <typename Resource, typename BoundedGraph>
void LabelDominanceAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::solve(){

   LabelDominanceAlgorithm<Resource,BoundedGraph>::vertexNonDominatedPathSet = vector<NonDominatedPathSet<Resource> >(LabelSettingAlgorithm<Resource,BoundedGraph>::graph->getNVertices());

    //---------------------------------------------------
    // Initial checks and prints
    //---------------------------------------------------
    LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
    LabelDominanceAlgorithm<Resource,BoundedGraph>::initialPrints();
    LabelDominanceAlgorithm<Resource,BoundedGraph>::initializeReverseCandidatePathsIfNotInitialized();
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
       if (LabelDominanceAlgorithm<Resource,BoundedGraph>::nNonDominated > 5 * LabelSettingAlgorithm<Resource,BoundedGraph>::labelMapMaxSize){
          LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
          LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
       }
        LabelSettingAlgorithm<Resource,BoundedGraph>::testIfMustBeStoppedBeforeEnd_withLabelMapMaxSize();
        LabelSettingAlgorithm<Resource,BoundedGraph>::midAlgorithmPrints();

        //---------------------------------------------------
        // Expansion
        //---------------------------------------------------
        LabelDominanceAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
        LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();

        //---------------------------------------------------
        // Test if it must be stopped
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateMinimumKnownCostAndLabelMap();

    }

    //---------------------------------------------------
    // Final prints and tests
    //---------------------------------------------------
    LabelDominanceAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
    LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
}




} /* namespace latticeRCSP */
