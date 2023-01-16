//
//  InterestingThresholdForProblemFinder.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 13/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__InterestingThresholdForProblemFinder__
#define __latticeRCSP_v2__InterestingThresholdForProblemFinder__

#include <stdio.h>
#include "../Context/ProblemRCSP.h"
#include "../Resources/Functors/ConstraintFunction.h"



namespace latticeRCSP {
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    class InterestingThresholdForProblemFinder {
        
        typedef ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor> InitialProblem;
        typedef ProblemRCSP<Resource, CostFunctor, NoConstraintFunction<Resource> , ContraintHeuristicFunctor> CostProblem;
        typedef ProblemRCSP<Resource, ContraintHeuristicFunctor, NoConstraintFunction<Resource> , ContraintHeuristicFunctor> ConstraintProblem;
        
        Path<Resource> solutionPath ;
        Path<Resource> costOptimalPath;
        Path<Resource> constraintOptimalPath;

        InitialProblem * initialProblem_m;
        double proportion;
        int maxSize;
        
    public:
        InterestingThresholdForProblemFinder(InitialProblem *);
        void setProportionOfThreshold(double);
        double getConstraintThreshold();
        void setMaxSize(int);
        Path<Resource> getCostOptimalPath() const;
       Path<Resource> getConstraintOptimalPath() const;
    private:
        template<typename ProblemSolved>
        double computeProblemSolution();
        


    };
}

#include "InterestingThresholdForProblemFinder.hpp"

#endif /* defined(__latticeRCSP_v2__InterestingThresholdForProblemFinder__) */
