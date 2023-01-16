//
//  ContextProblemRCSP.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ContextProblemRCSP__
#define __latticeRCSP_v2__ContextProblemRCSP__

#include <stdio.h>
#include "ContextGraph.h"
#include "../Graph/PathSet.h"
//#include "../Algorithm/CostConstraintDichotomyHeuristicToFindGoodSolution.h"

namespace latticeRCSP {
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    class CostConstraintDichotomyHeuristicToFindGoodSolution;
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    class ProblemRCSP{
    protected:
        //------------------
        // Problem statement
        //------------------
        ContextGraph<Resource> * contextGraph_m;
        //set<int> origin;
        //set<int> destination;
        
        //------------------
        // Solution
        //------------------
        PathSet<Resource, CostFunctor> * solutionSet;
        double * costOfWorstSolutionInSolutionSet;
        double lowerBoundOnCostOfWorstSolutionInOptimalSolutionSet;
        
    public:
        ProblemRCSP();
        ProblemRCSP(ContextGraph<Resource> *);
        ~ProblemRCSP();
        
        ProblemRCSP(int origin, int destination);
        ProblemRCSP(set<int> origin, set<int> destination, ContextGraph<Resource> *);
        
        //---------------------------------
        // Getters and setters
        //---------------------------------
        ContextGraph<Resource> * getContextGraph();
        
        //---------------------------------
        // Functions linked to solution set
        //---------------------------------
        void setLowerBoundOnCostOfWorstSolutionInOptimalSolutionSet(double);
        double getLowerBoundOnCostOfWorstSolutionInOptimalSolutionSet() const;
        void setNumberOfSolutionsSearched(int solutionSize);
        int getNumberOfSolutionsSearched() const;
        double const * getPointerToCostOfWorstSolutionInSolutionSet() const;
        int getSolutionSetSize() const;
        void addPotentialSolutionToSolutionSet(const Path<Resource> &);
        void clearSolutionSet();
        PathSet<Resource, CostFunctor> * getSolutionSet();
        void setSolutionSet(PathSet<Resource, CostFunctor> &);
        
        //---------------------------------
        // Functions linked to feasibility
        //---------------------------------
        pair<bool, Path<Resource> > testProblemFeasibilityAndGetFeasibleResource();
        
        //int constraintFeasibilityFunction(const Resource *) const;
        double costFunction(const Resource &) const;
        double constraintHeuristicKey(const Resource &) const;
        double keyForMeetExpansion(const Resource &) const;
        bool isResourceInfeasibleOrDominatedByASolution(const Resource &) const;
        
        
    };
}

#include "ProblemRCSP.hpp"

#endif /* defined(__latticeRCSP_v2__ContextProblemRCSP__) */
