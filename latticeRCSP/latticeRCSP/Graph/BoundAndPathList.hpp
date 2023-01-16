//
//  BoundAndPathList.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "BoundAndPathList.h"

namespace latticeRCSP {
    template<typename Resource>
    BoundAndPath<Resource>::BoundAndPath(){
        bound_m = Resource();
        path_m = Path<Resource>();
    }
    
    template<typename Resource>
    BoundAndPath<Resource>::BoundAndPath(Resource const & r, Path<Resource> const & p){
        bound_m = r;
        path_m = p;
    }
    
    template <typename Resource, typename Problem>
    BoundAndPathList<Resource, Problem>::BoundAndPathList(Problem * pb){
        pProblem_m = pb;
    }
    
    template <typename Resource, typename Problem>
    void BoundAndPathList<Resource, Problem>::insert(const BoundAndPath<Resource> & bp){
        typename list<BoundAndPath<Resource> >::iterator iter;
        for (iter = content.begin(); iter != content.end(); ) {
            if (bp.bound_m.isGeq(iter->bound_m)) {
                return;
            }
            
            if (bp.bound_m.isLeq(iter->bound_m)){
                typename list<BoundAndPath<Resource> >::iterator iterDel = iter;
                ++iter;
                content.erase(iterDel);
            } else {
                ++iter;
            }
        }
        content.push_back(bp);
    }
    
    template <typename Resource, typename Problem>
    bool BoundAndPathList<Resource, Problem>::testCompleteDominated(const Resource & r, bool isReverse) const{
        typename list<BoundAndPath<Resource> >::const_iterator iter;
        if (content.size() > 0) {
            for (iter = content.begin(); iter != content.end(); ++iter){
            	Resource completePath;
				if (isReverse){
            		completePath = iter->bound_m;
            		completePath.expand(r);
            	} else{
					completePath = r;
					completePath.expand(iter->bound_m);
            	}
                if (!pProblem_m->isResourceInfeasibleOrDominatedByASolution(completePath)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    template <typename Resource, typename Problem>
    bool BoundAndPathList<Resource, Problem>::testCompleteDominatedAndUpdateProblemSolution(const Path<Resource> & p, double &lowerBoundOnCostFromPath, bool isReverse) const {
        typename list<BoundAndPath<Resource> >::const_iterator iter;
        lowerBoundOnCostFromPath = INFINITY;
        bool result = true;
        if (content.size()>0){
            for (iter = content.begin(); iter != content.end(); ++iter){
                Resource completePathLB;
				if (isReverse){
            		completePathLB = iter->bound_m;
            		completePathLB.expand(p.getResource());
            	} else{
					completePathLB = p.getResource();
					completePathLB.expand(iter->bound_m);
            	}
//                = p.getResource();
//                completePathLB.expand(iter->bound_m);
                if (!pProblem_m->isResourceInfeasibleOrDominatedByASolution(completePathLB)) {
                    result = false;
                    double newLowerBound = pProblem_m->costFunction(completePathLB);
                    if (newLowerBound < lowerBoundOnCostFromPath) lowerBoundOnCostFromPath = newLowerBound;
                    
                    
                    Path<Resource> completePath = p;
                    if (iter->path_m.getDestination_neverReverse() != -1){
                        completePath.expand(iter->path_m);
                        pProblem_m->addPotentialSolutionToSolutionSet(completePath);
                    }
                }
            }
        }
        return result;
        
    }
    
}
