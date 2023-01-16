//
//  DistanceFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 23/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__DistanceFunctor__
#define __latticeRCSP_v2__DistanceFunctor__

#include <stdio.h>

namespace latticeRCSP {
    template <typename Resource>
    class DistanceFunctor {
    public:
        static double getDistance(const Resource &, const Resource &);
    };
    
    template <typename Resource>
    class DistanceZeroFunctor {
    public:
        static double getDistance(const Resource &, const Resource &);
    };
}

#include "DistanceFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__DistanceFunctor__) */
