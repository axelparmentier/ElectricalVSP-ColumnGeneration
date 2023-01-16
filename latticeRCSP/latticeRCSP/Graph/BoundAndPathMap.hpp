//
//  BoundAndPathMap.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 19/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "BoundAndPathMap.h"

namespace latticeRCSP {
template <typename Resource, typename Problem, typename ConditionalFunctor>
    CostConditionalFunctorRelation BoundAndPathMap<Resource, Problem, ConditionalFunctor>::costConditionalFunctorRelation_ = RELATION_BETWEEN_COST_AND_CONDITIONAL_FUNCTOR_UNDEFINED;
   
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    void BoundAndPathMap<Resource, Problem, ConditionalFunctor>::setCostConditionalFunctorRelation(CostConditionalFunctorRelation relation){
        costConditionalFunctorRelation_ = relation;
    }
    
template <typename Resource, typename Problem, typename ConditionalFunctor>
    BoundAndPathMap<Resource, Problem, ConditionalFunctor>::BoundAndPathMap(){
        nElements = 0;
    }
    
template <typename Resource, typename Problem, typename ConditionalFunctor>
    BoundAndPathMap<Resource, Problem, ConditionalFunctor>::BoundAndPathMap(vector<double> & weights, vector<Resource> & resourceVec, vector<Path<Resource> > & pathsVec){
        if (weights.size() != resourceVec.size() || weights.size() != pathsVec.size()) {
            throw "Weights, resources and paths vectors must have the same size"  ;
        }
        nElements = weights.size();
        weightVector = weights;
        boundVector = resourceVec;
        pathVector = pathsVec;
    }
    
    
    
//    template<typename BandPSet, typename setKeyFunctor>
//    void BoundAndPathMap<BandPSet, setKeyFunctor>::insert(const BandPSet & s){
//        mapSet.insert(pair<double, BandPSet>(d,s));
//    }
    
    template <typename Resource, typename Problem, typename ConditionalFunctor>
    vector<Path<Resource> > BoundAndPathMap<Resource, Problem, ConditionalFunctor>::getPathVector() const{
        return pathVector;
    }
    
template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool BoundAndPathMap<Resource, Problem, ConditionalFunctor>::testCompleteDominated(const Resource & resourceTested, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const graphWCB){
        double lowerBoundOnCostFromPath = 1;
        if (identifyFeasibleStateOrInfeasibility(resourceTested, graphWCB, lowerBoundOnCostFromPath) == -1){
            return true;
        } else return false;
    }
    
template <typename Resource, typename Problem, typename ConditionalFunctor>
    bool BoundAndPathMap<Resource, Problem, ConditionalFunctor>::testCompleteDominatedAndUpdateProblemSolution(Path<Resource> const & pathTested, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const graphWCB, double & lowerBoundOnCostFromPath){
        int stateIndex = identifyFeasibleStateOrInfeasibility(pathTested.getResource(), graphWCB, lowerBoundOnCostFromPath);
        if (stateIndex == -1){
            return true;
        } else {
            Path<Resource> completePath  = pathTested;
            try {
                for (int s = stateIndex; s < pathVector.size(); s++) {
                    if (pathVector[s].getDestination() != -1) {
                        completePath.expand(pathVector[s]);
                        graphWCB->getProblem()->addPotentialSolutionToSolutionSet(completePath);
                        throw s;
                    }
                }
            } catch (int s) {
            }
            return false;
        }
    }
    
    
template <typename Resource, typename Problem, typename ConditionalFunctor>
    int BoundAndPathMap<Resource, Problem, ConditionalFunctor>::identifyFeasibleStateOrInfeasibility(const Resource & resourceTested, GraphWithConditionalBounds<Resource, Problem, ConditionalFunctor> const * const graphWCB, double & lowerBoundOnCostFromPath){
//    On pourrait faire mieux en remplacant maxConditionalValue Par min(C_od^UB, maxCOnditionalValue)
	if (nElements == 0 || ConditionalFunctor::getKey(resourceTested) + weightVector[0] > graphWCB->getOriginDestinationMaxConditionalValue() + EPS) { // To be checked !!!
            //cout << "First stop" << endl;
            return -1;
        }
        else { // Find the largest non dominated cost
            
            int lowInd = 0;
            int upInd = nElements - 1;
            
            if (ConditionalFunctor::getKey(resourceTested) + weightVector[upInd] < graphWCB->getOriginDestinationMaxConditionalValue() + EPS) {
                lowInd = upInd;
            } else {
                while (upInd - lowInd > 1) {
                    int meanInd = (upInd + lowInd)/2;
                    if (ConditionalFunctor::getKey(resourceTested) + weightVector[meanInd] < graphWCB->getOriginDestinationMaxConditionalValue() + EPS) {
                        lowInd = meanInd ;
                    } else upInd = meanInd;
                }
            }
            Resource completePath = resourceTested;
            completePath.expand(boundVector[lowInd]);
            
            
            if (graphWCB->getProblem()->isResourceInfeasibleOrDominatedByASolution(completePath)){
            
                
                return -1;
            }
            else { // Among the feasible cost, find the lowest cost with resource constraint feasible
                upInd = lowInd;
                lowInd = 0;
                
                if (costConditionalFunctorRelation_ == COST_IS_CONDITIONAL_FUNCTOR) {
                    completePath = resourceTested;
                    completePath.expand(boundVector[lowInd]);
                    if (!graphWCB->getProblem()->isResourceInfeasibleOrDominatedByASolution(completePath)){
                        upInd = lowInd;
                    } else{
                        while (upInd - lowInd > 1) {
                            int meanInd = (upInd + lowInd)/2;
                            completePath = resourceTested;
                            completePath.expand(boundVector[meanInd]);
                            if (!graphWCB->getProblem()->isResourceInfeasibleOrDominatedByASolution(completePath)) upInd = meanInd;
                            else lowInd = meanInd;
                        }
                    }
                    
                    lowerBoundOnCostFromPath = ConditionalFunctor::getKey(resourceTested) + weightVector[upInd];
                    
                } else {
                    completePath = resourceTested;
                    completePath.expand(boundVector[upInd]);
                    lowerBoundOnCostFromPath = graphWCB->getProblem()->costFunction(completePath);
                    
                    if (costConditionalFunctorRelation_ == RELATION_BETWEEN_COST_AND_CONDITIONAL_FUNCTOR_UNDEFINED){
                        cout << endl << endl << endl << endl << "!!!!!!!!!!!!!!!!!!!!!!!!! Relation between cost and consitional functor not defined" << endl << "Not optimal test if cost is the conditional functor !!!!!!!!!!!!!" << endl << endl << endl;
                        costConditionalFunctorRelation_ = COST_IS_NOT_CONDITIONAL_FUNCTOR;
                        throw "!!!!!!!!!!!!!!!!!!!!!!!!! Not optimal test if cost is the conditional functor !!!!!!!!!!!!!";
                    }
                }
                
                return upInd;
            }
        }
    }
    
//    template<typename BandPSet, typename setKeyFunctor>
//    template<typename resource, typename costOptimizedKeyFunctor, typename constraintKeyFunctor>
//    bool BoundAndPathMap<BandPSet, setKeyFunctor>::testCompleteDominatedAndUpdateSolutionPathSet(label<resource> const & partialPath, pathSet<resource> & solutionToUpdate, double & costOfWorstSolutionInSolutionSet, double & pathEnumerationKey){
//
//        // It seems that it is good from the point of view of the update of the pathEnumerationKey
//        
//        
//// ----------------------------
//// Warning : the setKeyFunctor must have a method getConstraintKey()
////       - If it is the costFunctor => it must be initialized   costFunctor<resource>::setConstraintKey(labelAlgorithm.getPointerToCostOfWorstSolutionInSolutionSet
////       - If it is a constraintFucntor => initialized to something fixed
////        ----------------------------
//        
//        if (setKeyFunctor::getKey(partialPath.getResource()) + weightVector[0] > setKeyFunctor::getConstraintKey() + EPS) {
//            //cout << "First stop" << endl;
//            return true;
//        }
//        else { // Find the largest non dominated cost
//            
//            int lowInd = 0;
//            int upInd = nElements - 1;
//            
//            if (setKeyFunctor::getKey(partialPath.getResource()) + weightVector[upInd] < setKeyFunctor::getConstraintKey() + EPS) {
//                lowInd = upInd;
//            } else {
//                while (upInd - lowInd > 1) {
//                    int meanInd = (upInd + lowInd)/2;
//                    if (setKeyFunctor::getKey(partialPath.getResource()) + weightVector[meanInd] < setKeyFunctor::getConstraintKey() + EPS) {
//                        lowInd = meanInd ;
//                    } else upInd = meanInd;
//                }
//            }
////            resource completePath = partialPath;
////            completePath.expand(conditionalMeets[lowInd]);
//            
//            if (setVector[lowInd].template testCompleteDominatedAndUpdateSolutionPathSet<resource, costOptimizedKeyFunctor, constraintKeyFunctor>(partialPath, solutionToUpdate, costOfWorstSolutionInSolutionSet, pathEnumerationKey)){
//
//            //if (setVector[upInd].template testCompleteDominatedAndUpdateSolutionPathSet<resource, costOptimizedKeyFunctor, constraintKeyFunctor>(partialPath, solutionToUpdate, costOfWorstSolutionInSolutionSet, pathEnumerationKey)){
//                //cout << "second stop" << endl;
//                return true;
//            }
//            else { // Among the feasible cost, find the lowest cost with resource constraint feasible
//                upInd = lowInd;
//                lowInd = 0;
//                while (upInd - lowInd > 1) {
//                    int meanInd = (upInd + lowInd)/2;
////                    resource completePath = partialPath;
////                    completePath.expand(conditionalMeets[meanInd]);
//                    if (!setVector[meanInd].template testCompleteDominatedAndUpdateSolutionPathSet<resource, costOptimizedKeyFunctor, constraintKeyFunctor>(partialPath, solutionToUpdate, costOfWorstSolutionInSolutionSet, pathEnumerationKey)) upInd = meanInd;
//                    else lowInd = meanInd;
//                }
//                
////                lastCompleteDominatedTestKey = conditionalMeets[upInd].getCost() + partialPath.getCost(); //!!!!!!!!!!!!
//                return false;
//            }
//        }
//        
//    }
}
