/*
 * MeetAlgorithmFunctor.cpp
 *
 *  Created on: 13 août 2016
 *      Author: axelparmentier
 */

#include "MeetAlgorithmFunctor.h"
#include <limits>

namespace latticeRCSP {

template<typename Resource>
MeetAlgorithmFunctor<Resource>::MeetAlgorithmFunctor()
{
   isInserted = false;
   pathLength = std::numeric_limits<int>::max();

}

template<typename Resource>
void MeetAlgorithmFunctor<Resource>::removedFromMap(){
    isInserted = false;
}

template<typename Resource>
void MeetAlgorithmFunctor<Resource>::setVertexPositionInMap(const multimap<double,int>::iterator & it){
    isInserted = true;
    vertexPositionInMap = it;
}

template<typename Resource>
multimap<double,int>::iterator MeetAlgorithmFunctor<Resource>::getVertexPositionInMap() const{
    return vertexPositionInMap;
}

template<typename Resource>
void MeetAlgorithmFunctor<Resource>::setPathLength(int i){
    pathLength = i;
}

template<typename Resource>
int MeetAlgorithmFunctor<Resource>::getPathLength() const{
    return pathLength;
}

} /* namespace latticeRCSP */
