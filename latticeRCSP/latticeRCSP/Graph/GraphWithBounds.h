//
//  GraphWithBounds.h
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__GraphWithBounds__
#define __RCSP__GraphWithBounds__

#include <iostream>
#include <fstream>
#include <math.h>

#include "Graph.h"
#include "../GraphBuilder/GraphBuilder.h"
#include "PathSet.h"
#include "Path.h"
#include "MeetAlgorithmFunctors/MeetAlgorithmPathLengthFunctor.h"
#include "../Tools/ToolsForLatticeRCSP.h"
//#include "MeetAlgorithmFunctors/CostConstraintSimpleBoundMeetAlgorithmFunctor.h"
//#include "Resource.h"
//#include "instanceResourceReader.h"
#include "../Tools/ProgressBar.h"
#include "../Tools/ResultsPrinter.h"

#include "Results/GraphWithBoundsResults.h"

using namespace ToolsAxel;

namespace latticeRCSP {
template <typename Resource, typename Problem>
class GraphWithBounds : public Graph<Resource> {
protected:
    Problem * problem_m;
    
    //-------------------------------
    // Lower Bounds
    //-------------------------------
public:
    typedef enum {
      MEET_FUNCTOR_PATH_LENGTH,
      MEET_FUNCTOR_CONSTRAINT_HEURISTIC
    } MeetAlgorithmFunctor;

protected:
    static MeetAlgorithmFunctor meetAlgorithmFunctor_m;
public:
    static void setMeetAlgorithmFunctor(MeetAlgorithmFunctor f){meetAlgorithmFunctor_m = f;}

protected:
    bool isLowerBoundOnProblemSolutionSubPathsFromOriginToVertexSet_m;
    bool isLowerBoundOnProblemSolutionSubPathsFromVertexToDestinationSet_m;
    
//    bool isLatestBoundBuiltReverse_m;
    
    // Pointers enable to test if already set
    vector<Resource *> LowerBoundOnProblemSolutionSubPathsFromOriginToVertex;
    vector<Resource *> LowerBoundOnProblemSolutionSubPathsFromVertexToDestination;
    
    //-------------------------------
    // Upper bounds (candidate Paths)
    //-------------------------------
    bool isCandidatePathFromOriginToVertexSet_m;
    bool iscandidatePathFromVertexToDestinationSet_m;
    
    vector<Path<Resource> * > candidatePathFromOriginToVertex;
    vector<Path<Resource> * > candidatePathFromVertexToDestination;
    
    MeetAlgorithmResults meetResults;
    DijkstraLikeHeuristicResults dijstraHeuristicResults;
    

public:
    
    //-------------------------------
    // Meet Algorithm for lower bounds
    //-------------------------------
    void setDirectionLowerBoundsViaMeetShortestPathAlgorithm();
    
protected:
    virtual vector<Resource *> directionMeetShortestPathAlgorithm(); // Enables to choose the meetDominatedFunctor (this is the reaso why it is virtual : enables to choose something else in Conditionnal Graph)
    
    // template "isMeetDominatedAtVertexFunctor" enables to avoid expanding dominated lower bound
    // template "VertexResourceKeyFunctor" enable to select in which order resources are expanded in the resource algorithm
    
    template<typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> directionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & );
    
    template<typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> acyclicDirectionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & );
    
    template<typename isMeetDominatedAtVertexFunctor>
    vector<Resource *> withCycleDirectionMeetShortestPathAlgorithm(isMeetDominatedAtVertexFunctor & );
    
    template<typename isMeetDominatedAtVertexFunctor, typename VertexResourceKeyFunctor>
    vector<Resource *> withCycleDirectionMeetShortestPathAlgorithmWithMeetKeyFunctor(isMeetDominatedAtVertexFunctor & );

    // Dependencies of the function Update :
    template <typename isMeetDominatedAtVertexFunctor, typename VertexResourceKeyFunctor>
    void updateVertexFromArc(int arc, Resource * expandedVertexResource, VertexResourceKeyFunctor & expandedVertexInformation, Resource * & updatedVertexResource,  VertexResourceKeyFunctor & updatedVertexInformation, multimap<double,int> & vertexToBeExpanded, isMeetDominatedAtVertexFunctor & functor);
    
    //-------------------------------
    // Algorithm to compute good paths set for upper bound
    //-------------------------------
public:
    void setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(); // default heurstic choice
    
    template<typename heuristicToFindGoodPathsMinimizedFunctor>
    void setDirectionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(); // Direction choice

protected:
    template<typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(); // isPathDominatedAtVertexFunctor choce
    
    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor &); // cyclic or arcyclic choice

    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > acyclicDirectionDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor &);
    
    template<typename isPathDominatedAtVertexFunctor, typename heuristicToFindGoodPathsMinimizedFunctor>
    vector<Path<Resource> * > withCycleDirectionDijkstraLikeAlgorithm(isPathDominatedAtVertexFunctor &);

    
    
public:
    GraphWithBounds();
    GraphWithBounds(bool, Problem *); // empty graph
    GraphWithBounds(Graph<Resource> const *, Problem *);
    GraphWithBounds(Graph<Resource> &, Problem *);
    
    void initializeBooleans();
    
    ~GraphWithBounds();
    
    //-------------------------------
    // Getters and setters
    //-------------------------------
    Problem * getProblem() const;
    Resource getBoundFromVertexToDestination(int vertex) const;
    Resource getBoundFromOriginToVertex(int vertex) const;
    
    MeetAlgorithmResults getMeetResults() const;
    DijkstraLikeHeuristicResults getDijkstraHeuristicResults() const;

    //-------------------------------
    // Tests on bounds and paths
    //-------------------------------
    bool isDominated(int vertex, const Resource &) const;
    bool isDominated(const Resource &) const;
    bool getIsBoundSet() const;
    bool getIsGoodPathSet() const;
    bool getIsReverseBoundAndCandidatePathSet() const;
    bool getIsReverseCandidatePathSet() const;
    void setReverseBoundAndCandidatePath();
    void setReverseCandidatePath();
    void setProblem(Problem * p){problem_m = p;}
    
    //-------------------------------
    // Test path feasibility (for debug. must be at most one )
    //-------------------------------
    bool testPathFeasibility(string);//, Resource initialResource);
    int findArc(string arc,int Vertex);
    
    //-------------------------------
    // Directional Expand
    //-------------------------------
    static void directionalExpand(Resource & resourceExtended, Resource const & resourceAdded, bool isReverse);
    void directionalExpand(Resource & resourceExtended, Resource const & resourceAdded) const;

private:
    bool isDominated(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
    bool isPartialDominated(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
    void addToPotentialSolutionSetIfAtDestination(const Path<Resource> &) const;
    void completePathAndAddToPotentialSolutionSet(const Path<Resource> &) const;
public:
    bool testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
    bool testPartialDominatedAndUpdateProblemSolution(const Path<Resource> &, double & lowerBoundOnCostFromLabel) const;
};
    
    //---------------------------------------------------------------------------
    // Simple functor used inside function :
    //              directionMeetShortestPathAlgorithm()
    //---------------------------------------------------------------------------
    template <typename Resource, typename Problem>
    class CostConstraintSimpleBoundMeetAlgorithmFunctor {
        
        GraphWithBounds<Resource, Problem> * graph_m;
        
    public:
        CostConstraintSimpleBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> *);
        bool getKey(int vertex, Resource &) const;
    };
    //---------------------------------------------------------------------------
    template <typename Resource, typename Problem>
    class NoBoundMeetAlgorithmFunctor {
        
        GraphWithBounds<Resource, Problem> * graph_m;
        
    public:
        NoBoundMeetAlgorithmFunctor(GraphWithBounds<Resource, Problem> * );
        bool getKey(int vertex, Resource &) const;
    };
    
    
    //---------------------------------------------------------------------------
    // Simple functor used inside function :
    //              directionGoodPathsForUpperBoundsDijkstraLikeAlgorithm()
    //---------------------------------------------------------------------------
    template <typename Resource, typename Problem>
    class ProblemOfGraphHeuristicKeyFunctor {
        static bool graphCannotBeModified;
        static GraphWithBounds<Resource, Problem> * graph_m;
        
    public:
        static void setGraph(GraphWithBounds<Resource, Problem> *);
//        void addPotentialSolutionToSolutionSet(Path<Resource> &);
        static double getKey(Path<Resource> &);
        
        static void lockGraph();
        static void unlockGraph();
        
    };
}

#include "GraphWithBounds.hpp"

#endif /* defined(__RCSP__GraphWithBounds__) */
