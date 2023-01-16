//
//  LabelSettingAlgorithm.cpp
//  RCSP
//
//  Created by Axel Parmentier on 14/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

//#include "LabelSetting++gAlgorithm.h"
#include <set>
#include "../Tools/Console.h"

namespace latticeRCSP {

    template <typename Resource, typename BoundedGraph>
    LabelSettingAlgorithm<Resource, BoundedGraph>::LabelSettingAlgorithm(BoundedGraph * b) {
        graph = b;
        costOfWorstSolutionInSolutionSet = graph->getProblem()->getPointerToCostOfWorstSolutionInSolutionSet();
        results = LabelSettingAlgorithmResults();
        stoppedBeforeEnd = true;
        newSolutionFound = false;
        nLabelsTreated = 0;
        nLabelsExpanded = 0;
        nLabelsCut = 0;
        labelMapMaxSize = 10000000;
        isStopBoundActivated_m = false;
        stopBound_m = 0;
        isMemoryBounded = false;
        isMemoryBoundedExact = true;
        memoryBoundedMaxSize = 10000;
    }
    
//    template <typename Resource, typename BoundedGraph>
//    bool LabelSettingAlgorithm<Resource, BoundedGraph>::solve(){
//        try{
//            return solve(clock(), INFINITY, 0, INFINITY, false);
//        } catch (bool b){
//            cout << "Solved to optimum" << endl;
//            return b;
//        }
//        
//    }
//    
//    template <typename Resource, typename BoundedGraph>
//    bool LabelSettingAlgorithm<Resource, BoundedGraph>::solve(clock_t maxDuration, int numberOfIterationsBetweenTwoTests, double minImprovePercent, int maxNumberOfIterations, bool isClockTested){
    
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::solve(){
        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        t0 = clock();
        initialPrints();

        initializeReverseBoundsIfNotInitialized();
        initializeStatisticStructures();
        initializeAlgorithmStoppingStructures();

        //---------------------------------------------------
        // Algorithm
        //---------------------------------------------------
        t1 = clock();
        initializeLabelMapWithOrigin();
        
        stop = false; // Identify if the algorithm must be stopped (optimal path are already generated)
        while (!labelMap.empty() && !stop) {
            //---------------------------------------------------
            // Stopping before end tests
            //---------------------------------------------------
            testIfMustBeStoppedBeforeEnd();
            midAlgorithmPrints();
            
            //---------------------------------------------------
            // Expansion
            //---------------------------------------------------
            expandCurrentPath();
            memoryBoundedClean();
            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            updateMinimumKnownCostAndLabelMap();
            
        }
                        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        finalPrintsAndTests();
        updateProblemIfNewLowerBoundsWereFound();
        
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initializeReverseBoundsIfNotInitialized(){
        if (!graph->getIsReverseBoundAndCandidatePathSet()) {
        	auto logger = Console::getPtrLogger();
            logger->warn("Bounds must be set");
//            graph->setReverseBoundAndCandidatePath();
        }
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::updateMinimumKnownCostAndLabelMap(){
		if (labelMap.empty()) return;
        if (labelMap.begin()->first > *costOfWorstSolutionInSolutionSet + EPS) {
            stop = true;
        }
        size_t s1 = labelMap.size();
        labelMap.erase(labelMap.upper_bound(*costOfWorstSolutionInSolutionSet + EPS), labelMap.end());
        size_t s2 = labelMap.size();
        nLabelsCut += s1 - s2;
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::expandCurrentPath(){
        nLabelsExpanded++;
        
        typename multimap<double, Path<Resource > >::iterator currentIt= labelMap.begin();
        Path<Resource> currentPath = currentIt->second;
        labelMap.erase(currentIt);
        
        int currentVertex = currentPath.getDestination();
        if (graph->vertexType(currentVertex) != -1){// If it is not a destination
            for (int a = 0; a<graph->deltaPa(currentVertex).size(); a++) {
                nLabelsTreated++;
                int arc = graph->deltaPa(currentVertex)[a];
                Path<Resource> newPath = currentPath;
                newPath.expand(graph->getArc(arc));
                
                double lowerBoundOnCostFromLabel = INFINITY;
                
                if (!graph->testCompleteDominatedAndUpdateProblemSolution(newPath, lowerBoundOnCostFromLabel)){
                    labelMap.insert(pair<double,Path<Resource> >(lowerBoundOnCostFromLabel, newPath));
                } else nLabelsCut++;
            }
        }
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::updateProblemIfNewLowerBoundsWereFound(){
        if (! stoppedBeforeEnd){
            labelMap.clear();
        } else {
        	auto logger = Console::getPtrLogger();
            logger->info("Lower Bound on Solution: {}", labelMap.begin()->first);
            graph->getProblem()->setLowerBoundOnCostOfWorstSolutionInOptimalSolutionSet(labelMap.begin()->first);
        }
    }
    
    template <typename Resource, typename BoundedGraph>
        void LabelSettingAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests(){
//    	auto logger = Console::getPtrLogger();
//       logger->info("Label setting algorithm Performance: ");
       finalPrintsAndTests_shared();
       results.nSettingCuts = nLabelsCut;
    }

    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::finalPrintsAndTests_shared(){
    	auto logger = Console::getPtrLogger();

    	newSolutionFound = (*costOfWorstSolutionInSolutionSet < initialCost) || (graph->getProblem()->getSolutionSetSize() > initialSize);
       
        //Store results
        
        results.nLabelsTreated = nLabelsTreated;
        results.nLabelsExpanded = nLabelsExpanded;
        results.executionTime = clock() - t1;
        results.isSolvedToOptimal = !stoppedBeforeEnd;
        results.upperBound = * costOfWorstSolutionInSolutionSet;
        
        if (results.isSolvedToOptimal ){
            results.lowerBound = * costOfWorstSolutionInSolutionSet;
        } else {
            results.lowerBound = labelMap.begin()->first;
        }
        results.newSolutionsFound = newSolutionFound;
        
        // Print Results
       double LBprinted = LabelSettingAlgorithm<Resource,BoundedGraph>::stop ? labelMap.begin()->first : *costOfWorstSolutionInSolutionSet;

	   logger->info("--------------------------      Label Algorithm results        --------------------------");
       logger->info("| Extended |  Treated |    Cut   | Time (s) | Remaining|    LB    |    UB    |  #SolSet |");
	   logger->info("|{:>9} |{:>9} |{:>9} |{:09.3f} |{:>9} |{:>9} |{:>9} |{:>9} |", nLabelsExpanded, nLabelsTreated, \
			   nLabelsCut , (clock() - t1)/(float) CLOCKS_PER_SEC, labelMap.size(), LBprinted, \
			   *costOfWorstSolutionInSolutionSet, graph->getProblem()->getSolutionSetSize());
//        cout << nLabelsExpanded << " among " << nLabelsTreated << " in " << (clock() - t1)/(float) CLOCKS_PER_SEC  <<" s, and it remains  " << labelMap.size() << "partial labels" << endl;
//        cout << "Solution set size : " << graph->getProblem()->getSolutionSetSize()  <<  endl;
//        cout << "Cost of worst solution of solution set : " << * costOfWorstSolutionInSolutionSet << endl;
       logger->info("Were there new solutions found (boolean): {} ", newSolutionFound);
       if (isStopBoundActivated_m) logger->info("solved to optimality: {}", results.isSolvedToOptimal);
        
    }
    
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initialPrints(){
    	auto logger = Console::getPtrLogger();
	   logger->info("--------------------------      Label setting Algorithm        --------------------------");
#ifdef KEY_WITHOUT_BOUND
       cout << endl << "Keys without bounds: c(q_p)";
#endif
//        cout << endl << "------------------------------------------------------" << endl;
        logger->debug("Nota bene: problem solution set is not emptied at the beginning of the algorithm");
        if (isStopBoundActivated_m) logger->info("Stop bound activated: {}", stopBound_m);

    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initializeStatisticStructures(){
        
        results.initialize();
        labelMap.clear();
        
        nLabelsTreated = 0;
        nLabelsExpanded = 0;
        nLabelsCut = 0;
        
        stoppedBeforeEnd = false;
        newSolutionFound = false;
        
        initialCost = *costOfWorstSolutionInSolutionSet;
        initialSize = graph->getProblem()->getSolutionSetSize();
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::midAlgorithmPrints(){
        // to be removed !!!!!!!!!!!!!!!!
//        if (labelMap.begin()->first < lastValue - EPS) {
//            throw "aie";
//        } else {
//            lastValue = labelMap.begin()->first ;
//        } //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
    	if (nLabelsExpanded%10000 == 0) {
        	auto logger = Console::getPtrLogger();
    		if (nLabelsExpanded%200000 == 0){
    			logger->info("| Extended |  Treated |    Cut   | Time (s) | Remaining|    LB    |    UB    |  #SolSet |");
    		}
    		logger->info("|{:>9} |{:>9} |{:>9} |{:09.3f} |{:>9} |{:>9} |{:>9} |{:>9} |", nLabelsExpanded, nLabelsTreated, nLabelsCut, (clock() - t1)/(float) CLOCKS_PER_SEC, labelMap.size(), labelMap.begin()->first, *costOfWorstSolutionInSolutionSet, graph->getProblem()->getSolutionSetSize());
        }
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initializeLabelMapWithOrigin(){
        
        set<int> origin = graph->getOrigin();
        for (set<int>::iterator it = origin.begin(); it != origin.end(); ++it) {
            Path<Resource> r = Path<Resource>(graph->getIsReverse(), *it);
            //r.setCost(0);
            labelMap.insert(pair<double, Path<Resource> >(0,r));
        }
        
        isMemoryBoundedExact = true;

    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initializeAlgorithmStoppingStructures(){
        
        
        // There are no stopping structures in the mother class
        
    }

    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean(){
        if (!isMemoryBounded) return;

        int toBeDeleted = labelMap.size() - memoryBoundedMaxSize;
        if (toBeDeleted > 0){
            isMemoryBoundedExact = false;
            auto newEnd = labelMap.end();
            for (int i = 0; i < toBeDeleted; ++i)
            {
                --newEnd;
            }
            std::advance(newEnd, toBeDeleted);
            labelMap.erase(newEnd, labelMap.end());
        }
    }

    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::testIfMustBeStoppedBeforeEnd(){
        
        // There are no stopping tests in the mother classe
        // => see in inherited classes for specific tests
        //cout << "bad test if this prints" << endl;
        
    }
    
    template <typename Resource, typename BoundedGraph>
    LabelSettingAlgorithmResults LabelSettingAlgorithm<Resource, BoundedGraph>::getResults() const{
        return results;
    }
    
    template <typename Resource, typename BoundedGraph>
    bool LabelSettingAlgorithm<Resource, BoundedGraph>::getIsSolvedToOptimum(){
        return !stoppedBeforeEnd;
    }
    
    template <typename Resource, typename BoundedGraph>
    bool LabelSettingAlgorithm<Resource, BoundedGraph>::getHasNewSolutionsBeenFound(){
        return newSolutionFound;
    }
    
    
//-------------------------------------------------------------
// Inherited classe to stop algorithm at a given time
//-------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    LabelSettingAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::LabelSettingAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration0, int numberOfIterationsBetweenTwoTests0, double minImprovePercent0, int maxNumberOfIterations0) : LabelSettingAlgorithm<Resource, BoundedGraph>(graph){
        LabelSettingAlgorithm<Resource,BoundedGraph>::maxDuration = maxDuration0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::numberOfIterationsBetweenTwoTests = numberOfIterationsBetweenTwoTests0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::minImprovePercent = minImprovePercent0;
        LabelSettingAlgorithm<Resource,BoundedGraph>::maxNumberOfIterations = maxNumberOfIterations0;
        
    }
    
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm_withMaxTimeMonitoring<Resource, BoundedGraph>::solve(){
        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initialPrints();
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
            LabelSettingAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
            LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();
            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            LabelSettingAlgorithm<Resource,BoundedGraph>::updateMinimumKnownCostAndLabelMap();
            
        }
        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::initializeAlgorithmStoppingStructures_withMaxTimeMonitoring(){
        finalTime = maxDuration;
        if (maxDuration > 0) {
            finalTime = clock() + maxDuration;
        }
        
        costOfWorstSolutionAtLastTest = (*LabelSettingAlgorithm<Resource, BoundedGraph>::costOfWorstSolutionInSolutionSet);
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::testIfMustBeStoppedBeforeEnd_withMaxTimeMonitoring(){
        
        if (LabelSettingAlgorithm<Resource, BoundedGraph>::nLabelsExpanded % numberOfIterationsBetweenTwoTests ==0){
            if (clock() > finalTime  && finalTime > 0) {
                LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
                LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
            }
            
            if ((* LabelSettingAlgorithm<Resource, BoundedGraph>::costOfWorstSolutionInSolutionSet - costOfWorstSolutionAtLastTest) /(double) (max(costOfWorstSolutionAtLastTest, - costOfWorstSolutionAtLastTest)) < minImprovePercent ) {
                LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
                LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
            }
        }
        
        if (LabelSettingAlgorithm<Resource, BoundedGraph>::nLabelsExpanded % maxNumberOfIterations == 0) {
            LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
           LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
        }
    }
    
//-------------------------------------------------------------
// Inherited classe to stop algorithm if label Map becomes to large
//-------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    LabelSettingAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::LabelSettingAlgorithm_withLabelMapMaxSize(BoundedGraph * graph, unsigned int maxSize) : LabelSettingAlgorithm<Resource, BoundedGraph>(graph){
        LabelSettingAlgorithm<Resource,BoundedGraph>::labelMapMaxSize = maxSize;
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm_withLabelMapMaxSize<Resource, BoundedGraph>::solve(){
        //---------------------------------------------------
        // Initial checks and prints
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::t0 = clock();
        LabelSettingAlgorithm<Resource,BoundedGraph>::initialPrints();
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
            LabelSettingAlgorithm<Resource,BoundedGraph>::testIfMustBeStoppedBeforeEnd_withLabelMapMaxSize();
            LabelSettingAlgorithm<Resource,BoundedGraph>::midAlgorithmPrints();
            
            //---------------------------------------------------
            // Expansion
            //---------------------------------------------------
            LabelSettingAlgorithm<Resource,BoundedGraph>::expandCurrentPath();
            LabelSettingAlgorithm<Resource, BoundedGraph>::memoryBoundedClean();
            //---------------------------------------------------
            // Test if it must be stopped
            //---------------------------------------------------
            LabelSettingAlgorithm<Resource,BoundedGraph>::updateMinimumKnownCostAndLabelMap();
            
        }
        
        //---------------------------------------------------
        // Final prints and tests
        //---------------------------------------------------
        LabelSettingAlgorithm<Resource,BoundedGraph>::finalPrintsAndTests();
        LabelSettingAlgorithm<Resource,BoundedGraph>::updateProblemIfNewLowerBoundsWereFound();
    }
    
    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::setLabelMapMaxSize(unsigned int size){
        labelMapMaxSize = size;
    }

    template <typename Resource, typename BoundedGraph>
    void LabelSettingAlgorithm<Resource, BoundedGraph>::testIfMustBeStoppedBeforeEnd_withLabelMapMaxSize(){
//        cout << "test" << endl;
    	auto logger = Console::getPtrLogger();

    	if (isStopBoundActivated_m && *costOfWorstSolutionInSolutionSet + EPS < stopBound_m){
            LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
            LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
            logger->info("Stopped before end due to StopBound: {}, and found: {}", stopBound_m, *costOfWorstSolutionInSolutionSet);
    	}

        if (LabelSettingAlgorithm<Resource, BoundedGraph>::labelMap.size()>labelMapMaxSize) {
//            cerr << "error due to label map max size" << endl;
//            cerr << "LabelMap size: " << labelMap.size() << " and labelMapMaxSize: " << labelMapMaxSize << endl;
//            throw;
        	LabelSettingAlgorithm<Resource, BoundedGraph>::stop = true;
            LabelSettingAlgorithm<Resource, BoundedGraph>::stoppedBeforeEnd = true;
        	logger->error("Stopped before end because label map max size {} and current size {} ", labelMapMaxSize, LabelSettingAlgorithm<Resource, BoundedGraph>::labelMap.size());

        }
    }
    
}
