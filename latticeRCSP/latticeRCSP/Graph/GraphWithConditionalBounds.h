//
//  GraphWithConditionalBounds.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 17/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__GraphWithConditionalBounds__
#define __latticeRCSP_v2__GraphWithConditionalBounds__

#include <stdio.h>
#include <vector>
#include <list>

#include "Graph.h"
#include "ConditionalGraph.h"
#include "BoundAndPathMap.h"

#include "../Tools/ToolsForLatticeRCSP.h"
#include "../Algorithm/CostConstraintDichotomyHeuristicToFindGoodSolution.h"

#include "Results/GraphWithConditionalBoundsResults.h"


namespace latticeRCSP {
    
    
    
    
//    template <typename Resource, typename ConditionalFunctor, typename Problem>
//    class GraphWithConditionalBounds;

template <typename Resource, typename Problem, typename ConditionalFunctor>
class GraphWithConditionalBounds : public Graph<Resource> {
    Problem * problem_m;

    bool isConditionalBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
    bool isConditionalLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
    
    vector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> * > ConditionalBoundAndPathOnProblemSolutionSubPathsFromOriginToVertex;
    vector<BoundAndPathMap<Resource, Problem, ConditionalFunctor> * > ConditionalBoundAndPathOnProblemSolutionSubPathsFromVertexToDestination;
    
    ConditionalGraph<Resource, Problem, ConditionalFunctor> * conditionalGraphFromVertexToDestination;
    ConditionalGraph<Resource, Problem, ConditionalFunctor> * conditionalGraphFromOriginToVertex;
    
    //-----------------------------
    //Min conditionnal Value of subpath
    //-----------------------------
    ArcContext<Resource> * dummyArcForIntraVertexStateArc; // Enable to introduce or remove the intra-vertex state arc in the solution of SOTA
    
    bool isOriginDestinationMaxConditionalValueSet_m;
    bool isMinConditionnalValueOfSubpathFromOriginToVertexOfSolutionUpToDate_m;
    bool isMinConditionnalValueOfSubpathFromVertexToDestinationOfSolutionUpToDate_m;
    
    double OriginDestinationMaxConditionalValue;
    vector<double> minConditionnalValueOfSubpathFromOriginToVertexOfSolution;
    vector<double> minConditionnalValueOfSubpathFromVertexToDestinationOfSolution;
    
    //-----------------------------
    // Origin destination
    //-----------------------------
    vector<int> vertexOriginDestinationType;
    void initializeVertexOriginDestinationType();
    
public:
    //-----------------------------
    // Constructor and getters
    //-----------------------------
    GraphWithConditionalBounds(Graph<Resource> &, Problem *);
    virtual ~GraphWithConditionalBounds();
    
    GraphWithConditionalBoundsResults getGraphWithConditionalBoundsResults();
    list<ConditionalGraphBuildingResults> getDirectConditionalGraphBuildingResults();
    list<ConditionalGraphBuildingResults> getReverseConditionalGraphBuildingResults();
    int getSumOfConditionalGraphSize();
    
    //-----------------------------
    // Bounded Graph Domination Functions
    //-----------------------------
    
    Problem * getProblem() const;
    void setProblem(Problem * p){problem_m = p;}
    
    bool isDominated(int vertex, const Resource &) const;
    bool isDominated(const Resource &) const;
    bool getIsBoundSet() const;
    bool getIsReverseBoundAndCandidatePathSet() const;
    void setReverseBoundAndCandidatePath();

    
    //    bool isDominated(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
    //    void completePathAndAddToPotentialSolutionSet(const Path<Resource> &);
    bool testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
    //-----------------------------
    // Conditional bounds
    //-----------------------------
    bool getDirectionIsConditionalBoundSet();
    void setDirectionConditionalGraphsBoundsAndPath(double delta);
    void addDirectionConditionalPathOfDestinationAsProblemSolution();

    //-----------------------------
    //Min conditionnal Value of subpath
    //-----------------------------
public:
    
    bool isOriginDestinationMaxConditionalValueSet();
//    double testFeasibilityAndInitializeOriginDestinationMaxConditionalValue();
    void setOriginDestinationMaxConditionalValue(double);
    double getOriginDestinationMaxConditionalValue() const;
    
    double getVertexMinConditionnalValue(int);
    double getVertexMaxConditionnalValue(int);
    
    ArcContext<Resource> * getDummyArcContext();
    
    void updateMinConditionnalValueOfSubPath();
    
    bool isDestination(int) const;
    
private: // Dependenciese
    bool isDirectionMinConditionnalValueOfSubPathUpToDate();
    void setDirectionMinConditionnalValueOfSubPath();
    void setDirectionMinConditionnalValueOfSubPathFromConditionnalGraph(); //Solve SOTA in the sta
    void setDirectionMinConditionnalValueOfSubPathFromDijkstraAlgorithm();
    void setDirectionMinConditionnalValueOfSubPathFromAcyclicDynamicProgramming();
};

}
#include "MeetAlgorithmFunctors/CostConstraintConditionalBoundMeetAlgorithmFunctor.h"


#include "GraphWithConditionalBounds.hpp"


#endif /* defined(__latticeRCSP_v2__GraphWithConditionalBounds__) */
