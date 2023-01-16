/*
 * MeetAlgorithmConstraintHeuristicFunctor.h
 *
 *  Created on: 13 août 2016
 *      Author: axelparmentier
 */

#ifndef LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMCONSTRAINTHEURISTICFUNCTOR_H_
#define LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMCONSTRAINTHEURISTICFUNCTOR_H_

#include "MeetAlgorithmFunctor.h"

namespace latticeRCSP {

template<typename Resource, typename Problem>
class MeetAlgorithmConstraintHeuristicFunctor : public MeetAlgorithmFunctor<Resource>
{
   static Problem * problem_m;
   double key;

public:

   static void setProblem(Problem * pb){ problem_m = pb;}

   //--------------------------
   // Mandatory methods for meetAlgorithmFunctor
   //--------------------------
   MeetAlgorithmConstraintHeuristicFunctor();
   double getKey(){return key;}
   short int update(const MeetAlgorithmConstraintHeuristicFunctor<Resource, Problem> &, Resource *);
   void removedFromMap();
};

} /* namespace latticeRCSP */

#include "MeetAlgorithmConstraintHeuristicFunctor.hpp"

#endif /* LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMCONSTRAINTHEURISTICFUNCTOR_H_ */
