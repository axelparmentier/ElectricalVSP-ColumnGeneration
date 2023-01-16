//
//  BoundAndPathMap.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 19/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__BoundAndPathMap__
#define __latticeRCSP__BoundAndPathMap__



//#include <stdio.h>
//#include <vector>
//
//#include "Path.h"
#include "GraphWithConditionalBounds.h"


namespace latticeRCSP {
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    class GraphWithConditionalBounds;
    
    typedef enum{
        COST_IS_CONDITIONAL_FUNCTOR,
        COST_IS_NOT_CONDITIONAL_FUNCTOR,
        RELATION_BETWEEN_COST_AND_CONDITIONAL_FUNCTOR_UNDEFINED        
    } CostConditionalFunctorRelation;
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    class BoundAndPathMap {
        static CostConditionalFunctorRelation costConditionalFunctorRelation_;
        
        //map<double, BandPSet> mapSet;
        int nElements;
        bool isBoundSet;
        bool isPathSet;
        
        vector<double> weightVector;
        vector<Resource> boundVector;
        vector<Path<Resource> > pathVector;
        
        
    public:
        BoundAndPathMap();
        BoundAndPathMap(vector<double> & weights, vector<Resource> & resourceVec, vector<Path<Resource> > & pathsVec);
        
        vector<Path<Resource> > getPathVector() const;
        
        static void setCostConditionalFunctorRelation(CostConditionalFunctorRelation);
        
        bool testCompleteDominated(const Resource &, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const);
        bool testCompleteDominatedAndUpdateProblemSolution(Path<Resource> const &, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const, double & lowerBoundOnCostFromPath);
        
    private:
        int identifyFeasibleStateOrInfeasibility(const Resource &, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const, double & lowerBoundOnCostFromPath);
    };
}

//#include "GraphWithConditionalBounds.h"
#include "BoundAndPathMap.hpp"

#endif /* defined(__latticeRCSP__BoundAndPathMap__) */
