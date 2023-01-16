//
//  ContextProblemRCSP.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ProblemRCSP.h"

namespace latticeRCSP {
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::ProblemRCSP(){
        contextGraph_m = 0;
        solutionSet = new PathSet<Resource, CostFunctor>();
        solutionSet->setMaxSize(1);
        costOfWorstSolutionInSolutionSet = new double(INFINITY);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::ProblemRCSP(ContextGraph<Resource> * context0){
        contextGraph_m= context0;
//        origin = contextGraph_m->getGraph()->getSource();
//        destination = contextGraph_m->getGraph()->getSink();
        solutionSet = new PathSet<Resource, CostFunctor>();
        solutionSet->setMaxSize(1);
        costOfWorstSolutionInSolutionSet = new double(INFINITY);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::~ProblemRCSP(){
        delete solutionSet;
        delete costOfWorstSolutionInSolutionSet;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    ContextGraph<Resource> * ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getContextGraph(){
        return contextGraph_m;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    bool ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::isResourceInfeasibleOrDominatedByASolution(const Resource & r) const{
        return (!ConstraintFeasibilityFunctor::getKey(r)) || (CostFunctor::getKey(r) > * costOfWorstSolutionInSolutionSet - EPS);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    double ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::constraintHeuristicKey(const Resource & res) const{
        return ContraintHeuristicFunctor::getKey(res);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::clearSolutionSet(){
        *costOfWorstSolutionInSolutionSet = INFINITY;
        solutionSet->clear();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    PathSet<Resource, CostFunctor> * ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getSolutionSet(){
        return solutionSet;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::setSolutionSet(PathSet<Resource, CostFunctor> & newSolution){
        delete solutionSet;
        solutionSet = new PathSet<Resource, CostFunctor>(newSolution);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    double ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::costFunction(const Resource & r) const{
        return CostFunctor::getKey(r);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::setLowerBoundOnCostOfWorstSolutionInOptimalSolutionSet(double lowerBound){
        if (lowerBound > lowerBoundOnCostOfWorstSolutionInOptimalSolutionSet){
        lowerBoundOnCostOfWorstSolutionInOptimalSolutionSet = lowerBound;
        }
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    double ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getLowerBoundOnCostOfWorstSolutionInOptimalSolutionSet() const {
        return lowerBoundOnCostOfWorstSolutionInOptimalSolutionSet;
    }
    
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::setNumberOfSolutionsSearched(int size0){
        solutionSet->setMaxSize(size0);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    int ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getNumberOfSolutionsSearched() const{
        return solutionSet->getMaxSize();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    double const * ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getPointerToCostOfWorstSolutionInSolutionSet() const{
        return costOfWorstSolutionInSolutionSet;
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    int ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getSolutionSetSize() const{
        return solutionSet->size();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::addPotentialSolutionToSolutionSet(const Path<Resource> & candidatePath){
        
//#ifdef DEBUG_TESTS
//        if (!ConstraintFeasibilityFunctor::getKey(candidatePath.getResource())) {
//            throw "Infeasible";
//        }
//#endif
        if (ConstraintFeasibilityFunctor::getKey(candidatePath.getResource())) {
            * costOfWorstSolutionInSolutionSet = solutionSet->insert(candidatePath);
        }
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    pair<bool, Path<Resource> > ProblemRCSP<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::testProblemFeasibilityAndGetFeasibleResource(){
        CostConstraintDichotomyHeuristicToFindGoodSolution<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor> solver(this);
        return solver.isThereFeasibleSolutionsToProblem();
    }
}