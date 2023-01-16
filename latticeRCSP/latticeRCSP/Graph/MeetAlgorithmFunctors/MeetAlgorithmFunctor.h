/*
 * MeetAlgorithmFunctor.h
 *
 *  Created on: 13 août 2016
 *      Author: axelparmentier
 */

#ifndef LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMFUNCTOR_H_
#define LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMFUNCTOR_H_

#include <stdio.h>
#include <map>

using namespace std;

namespace latticeRCSP {

template<typename Resource>
class MeetAlgorithmFunctor
{
// Base class for meet algorithm functor: all the classes used in practice inherit from this one
protected:
   bool isInserted;
   multimap<double,int>::iterator vertexPositionInMap;
   int pathLength;

public:
   MeetAlgorithmFunctor();
   //--------------------------
   // Mandatory methods for meetAlgorithmFunctor
   //--------------------------
   void setPathLength(int);
   int getPathLength() const;
   multimap<double,int>::iterator getVertexPositionInMap() const;
   void setVertexPositionInMap(const multimap<double,int>::iterator &);
   void removedFromMap();
   //   The following methods must be defined in each child class
   //   double getKey() const;
   //   short int update(const MeetAlgorithmFunctor<Resource> &, Resource *);
};

} /* namespace latticeRCSP */

#include "MeetAlgorithmFunctor.hpp"

#endif /* LATTICERCSP_V2_GRAPH_MEETALGORITHMFUNCTORS_MEETALGORITHMFUNCTOR_H_ */
