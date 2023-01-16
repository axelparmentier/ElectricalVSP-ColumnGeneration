//
//  CostConstraintDichotomyHeuristicToFindGoodSolution.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__CostConstraintDichotomyHeuristicToFindGoodSolution__
#define __latticeRCSP_v2__CostConstraintDichotomyHeuristicToFindGoodSolution__

#include <stdio.h>
#include "../Algorithm/LabelSettingAlgorithm.h"
#include "../Context/ContextGraph.h"
#include "../Context/ProblemRCSP.h"
#include "../Resources/Functors/LinearCombinationFunction.h"
#include "../Resources/Functors/ConstraintFunction.h"
#include "../Algorithm/KshortestPathDijkstraLikeHeuristic.h"
#include "../Graph/GraphWithBounds.h"

#define DEFAULT_PROPORTION_GAP 0.05
#define DEFAULT_HEURISTIC_SOLUTION_SIZE_FACTOR 5

namespace latticeRCSP {
//    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
//    class ProblemRCSP;
//    
//    template <typename Resource, typename BoundedGraph>
//    class LabelSettingAlgorithm;
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ConstraintHeuristicFunctor>
    class CostConstraintDichotomyHeuristicToFindGoodSolution {
        
        typedef ProblemRCSP< Resource,  CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor> InitialProblemType;
        typedef LinearCombinationFunction<Resource, ConstraintHeuristicFunctor, CostFunctor> heuristicCombinationFunctor;

        typedef ProblemRCSP<Resource, heuristicCombinationFunctor, NoConstraintFunction<Resource>, ConstraintHeuristicFunctor> HeuristicProblemType;
        
        typedef GraphWithBounds<Resource, HeuristicProblemType> BoundedGraphType;
        
        typedef KshortestPathDijkstraLikeHeuristic<Resource, heuristicCombinationFunctor> DijkstraHeuristicType;
        
        InitialProblemType * initialProblem;
        HeuristicProblemType * heuristicProblem;
        PathSet<Resource, heuristicCombinationFunctor> * heuristicSolution;
        
        GraphWithBounds<Resource, HeuristicProblemType> * boundedGraph;
        
        double proportionGap;
        int heuristicSolutionSizeFactorOfInitialSolutionSize;
        
        
    public:
        CostConstraintDichotomyHeuristicToFindGoodSolution(ProblemRCSP< Resource,  CostFunctor, ConstraintFeasibilityFunctor, ConstraintHeuristicFunctor> *);
        ~CostConstraintDichotomyHeuristicToFindGoodSolution();

        pair<bool, Path<Resource> > isThereFeasibleSolutionsToProblem();
        
        short int solve();
        void setProportionGap(double);
        void setHeuristicSolutionSizeFactorOfInitialSolutionSize(int);
        
        void testIfFeasibleSolutionsExistAndGetGapBetweenUnconstrainedAndFeasible(double & unconstrainedCost, double & constrainedCost, bool & feasibleSolutionExist, bool & solvedToOptimality);
        
    private:
        bool testIfFeasibleSolutionsExistNormalizeConstraint();
        bool testIfUnconstrainedSolutionAreFeasibleAndNormalizeCost();
        
        void solveLabelSettingLinearCombinationProblemWithProportionOfConstraint(double proportion);        
        int testAndAddSolutionsOfHeuristicProblemToInitialProblemSolutions();
        
        void solveDijkstraLikeLinearCombinationProblemWithProportion(double proportion);
        int testAndAddSolutionsOfDijkstraToInitialProblemSolutions();
    };
    
}

#include "CostConstraintDichotomyHeuristicToFindGoodSolution.hpp"


#endif /* defined(__latticeRCSP_v2__CostConstraintDichotomyHeuristicToFindGoodSolution__) */
