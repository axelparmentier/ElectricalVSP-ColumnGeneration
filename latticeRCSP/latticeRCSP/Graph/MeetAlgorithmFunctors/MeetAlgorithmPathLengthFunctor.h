//
//  MeetAlgorithmPathLengthFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 11/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__MeetAlgorithmPathLengthFunctor__
#define __latticeRCSP_v2__MeetAlgorithmPathLengthFunctor__

#include <stdio.h>
#include <map>

#include "MeetAlgorithmFunctor.h"


using namespace std;

namespace latticeRCSP {
    template<typename Resource>
    class MeetAlgorithmPathLengthFunctor : public MeetAlgorithmFunctor<Resource> {
        static int maxLength;
        
//        int pathLength;
        
//        bool isInserted;
//        multimap<double,int>::iterator vertexPositionInMap;
        
    public:
        static void setMaxLength(int);

        //--------------------------
        // Mandatory methods for meetAlgorithmFunctor
        //--------------------------
        MeetAlgorithmPathLengthFunctor();
//        void setPathLength(int);
//        int getPathLength() const;
        double getKey() const;
        short int update(const MeetAlgorithmPathLengthFunctor<Resource> &, Resource *);
//        multimap<double,int>::iterator getVertexPositionInMap() const;
//        void setVertexPositionInMap(const multimap<double,int>::iterator &);
        void removedFromMap();
        
        

        
    };
}

#include "MeetAlgorithmPathLengthFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__MeetAlgorithmPathLengthFunctor__) */
