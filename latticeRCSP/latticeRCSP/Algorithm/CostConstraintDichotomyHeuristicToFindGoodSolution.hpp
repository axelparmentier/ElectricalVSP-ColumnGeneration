//
//  CostConstraintDichotomyHeuristicToFindGoodSolution.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "CostConstraintDichotomyHeuristicToFindGoodSolution.h"

namespace latticeRCSP {
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::CostConstraintDichotomyHeuristicToFindGoodSolution(InitialProblemType * problem0){
        initialProblem = problem0;
        
        heuristicProblem = new HeuristicProblemType(initialProblem->getContextGraph());

        Graph<Resource> gr = *initialProblem->getContextGraph()->getGraph();
        boundedGraph = new GraphWithBounds<Resource, HeuristicProblemType>(gr, heuristicProblem);
        
        proportionGap = DEFAULT_PROPORTION_GAP;
        heuristicSolutionSizeFactorOfInitialSolutionSize = DEFAULT_HEURISTIC_SOLUTION_SIZE_FACTOR;
        setHeuristicSolutionSizeFactorOfInitialSolutionSize(heuristicSolutionSizeFactorOfInitialSolutionSize);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::~CostConstraintDichotomyHeuristicToFindGoodSolution(){
        delete heuristicProblem;
        delete boundedGraph;
        //delete heuristicSolution;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    short int CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::solve(){
        cout << endl << "------------------------------------------" << endl;
        cout << "Dichotomy heuristic to find good solutions";
        cout << endl << "------------------------------------------" << endl;
        
        heuristicProblem->setNumberOfSolutionsSearched(initialProblem->getNumberOfSolutionsSearched() * heuristicSolutionSizeFactorOfInitialSolutionSize);

        boundedGraph->setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        boundedGraph->setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
        boundedGraph->reverseGraph();
        
        if (!testIfFeasibleSolutionsExistNormalizeConstraint()) {
            cout << "Problem admits no solutions" << endl;
            return -1;
        } else{
            cout << endl << "--------" << endl;
            cout << "Dichotomy heuristic result : ";
            cout << endl << "--------" << endl;
            cout << "Problem is feasible" << endl;
        }
        
        if (testIfUnconstrainedSolutionAreFeasibleAndNormalizeCost()) {
            cout << endl << "--------" << endl;
            cout << "Dichotomy heuristic result : ";
            cout << endl << "--------" << endl;
            cout << "Unconstrained problem solutions are feasible : solved to optimum" << endl;
            return 1;
        } else {
            cout << endl << "--------" << endl;
            cout << "Problem is feasible : Dichotomy heuristic  starts to dichotomy: ";
            cout << endl << "--------" << endl;
        }
        
        
        double upConstraintProportion = 1;
        double lowConstraintProportion = 0;
        

        while (upConstraintProportion - lowConstraintProportion > proportionGap) {
            double meanConstraintProportion = (upConstraintProportion + lowConstraintProportion)/2;
            solveDijkstraLikeLinearCombinationProblemWithProportion(meanConstraintProportion);
            cout << "Avant test" << endl;
            if (testAndAddSolutionsOfDijkstraToInitialProblemSolutions() > 0){
                cout << "apres test" << endl;
                upConstraintProportion = meanConstraintProportion;
            } else lowConstraintProportion = meanConstraintProportion;
        }
        
        
        cout << endl << "--------" << endl;
        cout << "Dichotomy heuristic result : ";
        cout << endl << "--------" << endl;
        cout << "Final proportion of contraint: " << upConstraintProportion << endl;
        cout << "Numbers of solutions in solution set: " << initialProblem->getSolutionSetSize() << endl;
        cout << "Cost of worst solution in solution set : " << *(initialProblem->getPointerToCostOfWorstSolutionInSolutionSet()) << endl;
        
        return 0;
        
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    void CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::setProportionGap(double newGap){
        proportionGap = newGap;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    void CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::setHeuristicSolutionSizeFactorOfInitialSolutionSize(int newFactor) {
        heuristicSolutionSizeFactorOfInitialSolutionSize = newFactor;
        heuristicProblem->setNumberOfSolutionsSearched(newFactor * initialProblem->getNumberOfSolutionsSearched());
    }
    
    
//    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    bool CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::testIfFeasibleSolutionsExistNormalizeConstraint(){
        solveLabelSettingLinearCombinationProblemWithProportionOfConstraint(1);
//        Path<Resource> & sol = *;
        testAndAddSolutionsOfHeuristicProblemToInitialProblemSolutions();
        
        return (initialProblem->getSolutionSetSize() == initialProblem->getNumberOfSolutionsSearched());
//        return ConstraintFeasibilityFunctor::getKey(heuristicProblem->getSolutionSet()->getMinimumKeySolution()->getResource());

    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    void CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::testIfFeasibleSolutionsExistAndGetGapBetweenUnconstrainedAndFeasible(double &unconstrainedCost, double & feasibleSolutionKnownCost, bool & feasibleSolutionExist, bool & solvedToOptimality){
        
        boundedGraph->setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        boundedGraph->setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();
        boundedGraph->reverseGraph();
        
        feasibleSolutionExist = testIfFeasibleSolutionsExistNormalizeConstraint();
        feasibleSolutionKnownCost = *(initialProblem->getPointerToCostOfWorstSolutionInSolutionSet());
        
        solvedToOptimality = testIfUnconstrainedSolutionAreFeasibleAndNormalizeCost();
        unconstrainedCost = *(heuristicProblem->getPointerToCostOfWorstSolutionInSolutionSet());
    }
//
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    void CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::solveLabelSettingLinearCombinationProblemWithProportionOfConstraint(double proportion){
        
        cout << endl << "--- Dichtomy ---" << endl << "Proportion of constraint: " << proportion << endl;
        heuristicCombinationFunctor::setProportionOfFirstFunctor(proportion);
        LabelSettingAlgorithm<Resource, BoundedGraphType> labelSettingSolver(boundedGraph);
        heuristicProblem->clearSolutionSet();
        labelSettingSolver.solve();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    bool CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::testIfUnconstrainedSolutionAreFeasibleAndNormalizeCost(){
        solveLabelSettingLinearCombinationProblemWithProportionOfConstraint(0);
        
        
        if (testAndAddSolutionsOfHeuristicProblemToInitialProblemSolutions() >= initialProblem->getNumberOfSolutionsSearched()) {
            return true;
        }
        return false;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    int CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::testAndAddSolutionsOfHeuristicProblemToInitialProblemSolutions(){
        
        int numberOfFeasibleSolution = 0;
        
        map<string, Path<Resource> * > const * heuristicPaths = heuristicProblem->getSolutionSet()->getSolution();
        for (typename map<string, Path<Resource> * >::const_iterator it = heuristicPaths->begin(); \
             it != heuristicPaths->end(); ++it) {
            
            Path<Resource> candidate = *(it->second);
            if (ConstraintFeasibilityFunctor::getKey(candidate.getResource())){
                numberOfFeasibleSolution++;
                
                initialProblem->addPotentialSolutionToSolutionSet(candidate);
            } 
        }
        
        cout << endl << "--- Dichtomy ---" << endl << "Number of feasible solution found: " << numberOfFeasibleSolution << endl;
        return numberOfFeasibleSolution;
    }
    // return the number of solutions that are feasible
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    void CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::solveDijkstraLikeLinearCombinationProblemWithProportion(double proportion){
        
        heuristicCombinationFunctor::setProportionOfFirstFunctor(proportion);
        DijkstraHeuristicType instanceSolver( boundedGraph, initialProblem->getNumberOfSolutionsSearched() * heuristicSolutionSizeFactorOfInitialSolutionSize);
        instanceSolver.solve();
        cout << "Apres algorithme" << endl;
//        PathSet<Resource, heuristicCombinationFunctor> test = instanceSolver.getSolution();
//        heuristicSolution = test;
        delete heuristicSolution;
        heuristicSolution = new PathSet<Resource, heuristicCombinationFunctor>(instanceSolver.getSolution());
        cout << "Apres getSolution" << endl;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    int CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::testAndAddSolutionsOfDijkstraToInitialProblemSolutions(){
        int numberOfFeasibleSolution = 0;
        
        map<string, Path<Resource> * > const * heuristicPaths = heuristicSolution->getSolution();
        for (typename map<string, Path<Resource> * >::const_iterator it = heuristicPaths->begin(); \
             it != heuristicPaths->end(); ++it) {
            if (ConstraintFeasibilityFunctor::getKey(it->second->getResource())){
                numberOfFeasibleSolution++;
                Path<Resource> candidate = *(it->second);
                initialProblem->addPotentialSolutionToSolutionSet(candidate);
            }
        }
        
        cout << endl << "--- Dichtomy ---" << endl << "Number of feasible solution found: " << numberOfFeasibleSolution << endl;
        return numberOfFeasibleSolution;
    }
    
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    pair<bool, Path<Resource> > CostConstraintDichotomyHeuristicToFindGoodSolution< Resource, CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor>::isThereFeasibleSolutionsToProblem(){
        solveLabelSettingLinearCombinationProblemWithProportionOfConstraint(1);
        //        Path<Resource> & sol = *;
        Path<Resource> potentialSolution = * heuristicProblem->getSolutionSet()->getMinimumKeySolution();
        cout << "!!!! Feasibility test: " << ConstraintFeasibilityFunctor::getKey(potentialSolution.getResource()) << endl;
        cout << "Minimum constraint " << ConstraintHeuristicFunctor::getKey(potentialSolution.getResource()) << endl;
        
        return pair<bool, Path<Resource> >(ConstraintFeasibilityFunctor::getKey(potentialSolution.getResource()), potentialSolution);
    }
}