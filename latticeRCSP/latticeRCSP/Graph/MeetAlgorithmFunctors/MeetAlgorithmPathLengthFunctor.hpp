//
//  MeetAlgorithmPathLengthFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 11/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "MeetAlgorithmPathLengthFunctor.h"
#include <limits>

namespace latticeRCSP {
    template<typename Resource>
    int MeetAlgorithmPathLengthFunctor<Resource>::maxLength = std::numeric_limits<int>::max();
    
    template<typename Resource>
    MeetAlgorithmPathLengthFunctor<Resource>::MeetAlgorithmPathLengthFunctor() : MeetAlgorithmFunctor<Resource>() {
//        pathLength = std::numeric_limits<int>::max();
//        isInserted = false;
//        vertexPositionInMap = nullptr;
        
    }
    
//    template<typename Resource>
//    int MeetAlgorithmPathLengthFunctor<Resource>::getPathLength() const{
//        return pathLength;
//    }

    
    template<typename Resource>
    double MeetAlgorithmPathLengthFunctor<Resource>::getKey() const{
        return MeetAlgorithmFunctor<Resource>::pathLength;
    }
    
    template<typename Resource>
    short int MeetAlgorithmPathLengthFunctor<Resource>::update(const MeetAlgorithmPathLengthFunctor<Resource> & functor, Resource * r){
        short int result = 0;
        int candidateLength = functor.getPathLength() + 1 ;
        
        if (candidateLength < maxLength) {
            result++;
            if (candidateLength < MeetAlgorithmFunctor<Resource>::pathLength) MeetAlgorithmFunctor<Resource>::pathLength = candidateLength;
            if (MeetAlgorithmFunctor<Resource>::isInserted) result++;
        }
        
//        if (candidateLength< pathLength && candidateLength < maxLength) {
//            result++;
//            pathLength = candidateLength;
//            if (isInserted) result++;
//        }
        MeetAlgorithmFunctor<Resource>::isInserted = true;
        return result;
    }
    
    template<typename Resource>
    void MeetAlgorithmPathLengthFunctor<Resource>::removedFromMap(){
       MeetAlgorithmFunctor<Resource>::pathLength = std::numeric_limits<int>::max();
        MeetAlgorithmFunctor<Resource>::removedFromMap();
    }
    
//    template<typename Resource>
//    void MeetAlgorithmPathLengthFunctor<Resource>::setVertexPositionInMap(const multimap<double,int>::iterator & it){
//        isInserted = true;
//        vertexPositionInMap = it;
//    }
//
//    template<typename Resource>
//    multimap<double,int>::iterator MeetAlgorithmPathLengthFunctor<Resource>::getVertexPositionInMap() const{
//        return vertexPositionInMap;
//    }
    
//    template<typename Resource>
//    void MeetAlgorithmPathLengthFunctor<Resource>::setPathLength(int i){
//        pathLength = i;
//    }
//
//
    template<typename Resource>
    void MeetAlgorithmPathLengthFunctor<Resource>::setMaxLength(int i){
        maxLength = i;
    }
}
