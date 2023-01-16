//
//  InterestingThresholdForProblemFinder.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 13/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "InterestingThresholdForProblemFinder.h"
#include "../Graph//GraphWithBounds.h"

namespace latticeRCSP {
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::InterestingThresholdForProblemFinder(InitialProblem * problem_0){
        proportion = 0.5;
        maxSize = 1000000;
        initialProblem_m = problem_0;
        solutionPath = Path<Resource>();
        costOptimalPath = Path<Resource>();
        constraintOptimalPath = Path<Resource>();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::setProportionOfThreshold(double prop){
        proportion = prop;
        solutionPath = Path<Resource>();
       costOptimalPath = Path<Resource>();
       constraintOptimalPath = Path<Resource>();
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    Path<Resource> InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getConstraintOptimalPath() const {
       return constraintOptimalPath;
    }

    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
        Path<Resource> InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getCostOptimalPath() const {
           return costOptimalPath;
        }

    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    void InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::setMaxSize(int maxSize_l){
       maxSize = maxSize_l;
    }

    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    double InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::getConstraintThreshold(){
        cout << endl<< "-------------------" << endl;
        cout << "Constraint threshold algorithms: ";
        cout << endl<< "-------------------" << endl;
        double constraintProblemVal = computeProblemSolution<ConstraintProblem>();
        constraintOptimalPath = solutionPath;
        double costProblemVal = computeProblemSolution<CostProblem>();
        costOptimalPath = solutionPath;
        
        cout << endl<< "-------------------" << endl;
        cout << "Constraint threshold value: ";
        cout << endl<< "-------------------" << endl;
        cout << "Unconstrainted solution constraint heuristic: " << costProblemVal << endl;
        cout << "Minimum constraint heuristic: " << constraintProblemVal << endl;
        cout << "Threshold chosen : " << ((1 -proportion)*constraintProblemVal+ proportion * costProblemVal)  << endl;
        return ((1 -proportion)*constraintProblemVal+ proportion * costProblemVal);
    }
    
    template<typename Resource, typename CostFunctor, typename ConstraintFeasibilityFunctor, typename ContraintHeuristicFunctor>
    template<typename ProblemSolved>
    double InterestingThresholdForProblemFinder<Resource, CostFunctor, ConstraintFeasibilityFunctor, ContraintHeuristicFunctor>::computeProblemSolution() {
        
        ProblemSolved problem(initialProblem_m->getContextGraph());
        problem.setNumberOfSolutionsSearched(1);
        Graph<Resource> graph = *problem.getContextGraph()->getGraph();
        GraphWithBounds<Resource, ProblemSolved> instanceGWB(graph, &problem);
        instanceGWB.setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
        instanceGWB.setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm();

        instanceGWB.reverseGraph();
        
        LabelSettingAlgorithm_withLabelMapMaxSize<Resource, GraphWithBounds<Resource, ProblemSolved> > instanceSetting(& instanceGWB, maxSize);
        instanceSetting.solve();
        
        solutionPath = *problem.getSolutionSet()->getMinimumKeySolution();
        cout << "Path:"  << solutionPath.getID() << endl;
        Path<Resource> solutionPathRebuild = initialProblem_m->getContextGraph()->rebuildPathFromString(solutionPath.getID());
        cout << "Constraint:" << ContraintHeuristicFunctor::getKey(solutionPath.getResource()) << " check by rebuilding " <<  ContraintHeuristicFunctor::getKey(solutionPathRebuild.getResource())  << endl;
        cout << "Cost:" << CostFunctor::getKey(solutionPath.getResource()) << "check " << CostFunctor::getKey(solutionPathRebuild.getResource()) << endl;
        
        return ContraintHeuristicFunctor::getKey(solutionPath.getResource());
        
    }
    
    

}
