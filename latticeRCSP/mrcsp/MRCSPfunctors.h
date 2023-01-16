/*
 * MRCSPfunctors.h
 *
 *  Created on: 24 mars 2017
 *      Author: axelparmentier
 */

#ifndef SRC_MRCSP_MRCSPFUNCTORS_H_
#define SRC_MRCSP_MRCSPFUNCTORS_H_

namespace mrcsp {

template <typename Resource, typename CostConstaintFunctions>
class MRCSPcostFunctor{
public:
   static double getKey(const Resource & r){
      return CostConstaintFunctions::getCost(r);
   }
};

template <typename Resource, typename CostConstaintFunctions>
class MRCSPconstraintFeasibilityFunctor{
public:
   static bool getKey(const Resource & r){
      return CostConstaintFunctions::isConstraintSatisfied(r);
   }
};

template <typename Resource, typename CostConstaintFunctions>
class MRCSPconstraintHeuristicFunctor{
public:
   static double getKey(const Resource & r){
      return CostConstaintFunctions::getResourceConstraintHeuristic(r);
   }
};

}

#endif /* SRC_MRCSP_MRCSPFUNCTORS_H_ */
