//
//  DistanceFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 23/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "DistanceFunctor.h"

namespace latticeRCSP {
    template <typename Resource>
    double DistanceFunctor<Resource>::getDistance(Resource const & r1, Resource const & r2){
        return r1.distance(r2);
    }
    
    template <typename Resource>
    double DistanceZeroFunctor<Resource>::getDistance(Resource const & r1, Resource const & r2){
        return 0;
    }
    
    
}