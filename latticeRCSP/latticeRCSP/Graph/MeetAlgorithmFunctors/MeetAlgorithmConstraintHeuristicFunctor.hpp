/*
 * MeetAlgorithmConstraintHeuristicFunctor.cpp
 *
 *  Created on: 13 août 2016
 *      Author: axelparmentier
 */

#include "MeetAlgorithmConstraintHeuristicFunctor.h"
#include <math.h>

namespace latticeRCSP {

template<typename Resource, typename Problem>
Problem * MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem>::problem_m = 0;

template<typename Resource, typename Problem>
MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem>::MeetAlgorithmConstraintHeuristicFunctor() : MeetAlgorithmFunctor<Resource>()
{
   key = INFINITY;
}

template<typename Resource, typename Problem>
short int MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem>::update(const MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem> & functor, Resource * r){
   short int result = 1;
   double newKey = problem_m->constraintHeuristicKey(*r);
   if (newKey < key) key = newKey;
   if (MeetAlgorithmFunctor<Resource>::isInserted) result++;

   MeetAlgorithmFunctor<Resource>::isInserted = true;
   return result;
}

template<typename Resource, typename Problem>
void MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem>::removedFromMap(){
   key = INFINITY;
   MeetAlgorithmFunctor<Resource>::removedFromMap();
}
//MeetAlgorithmConstraintHeuristicFunctor::~MeetAlgorithmConstraintHeuristicFunctor()
//{
//   // TODO Auto-generated destructor stub
//}

} /* namespace latticeRCSP */
