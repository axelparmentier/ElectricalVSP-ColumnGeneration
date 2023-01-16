//
//  BoundAndPathList.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__BoundAndPathList__
#define __latticeRCSP_v2__BoundAndPathList__

#include <stdio.h>
#include <list>
#include "Path.h"


namespace latticeRCSP {
    
    template <typename Resource>
    struct BoundAndPath{
        Resource bound_m;
        Path<Resource> path_m;
        BoundAndPath();
        BoundAndPath(Resource const &, Path<Resource> const &);
    };
    
    template <typename Resource, typename Problem>
    class BoundAndPathList {
    protected:
        Problem * pProblem_m;
        
        list<BoundAndPath<Resource> > content;
        
        
    public:
        BoundAndPathList(Problem *);
        
        void insert(BoundAndPath<Resource> const &);
        
        bool testCompleteDominated(const Resource &, bool isReverse) const;
        bool testCompleteDominatedAndUpdateProblemSolution(Path<Resource> const &, double & lowerBoundOnCostFromPath, bool isReverse) const;
    };
}
#include "BoundAndPathList.hpp"

#endif /* defined(__latticeRCSP_v2__BoundAndPathList__) */
