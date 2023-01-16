//
//  ResourceProductFunctions.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceProductFunctions.h"

namespace latticeRCSP {
    template<typename Resource1Functor, typename ResourceProductConsidered>
    double ResourceProductFunctor_Resource1<Resource1Functor, ResourceProductConsidered>::getKey(const ResourceProductConsidered & rp){
        return Resource1Functor::getKey(rp.getResource1());
    }
    
    template<typename Resource2Functor, typename ResourceProductConsidered>
    double ResourceProductFunctor_Resource2<Resource2Functor, ResourceProductConsidered>::getKey(const ResourceProductConsidered & rp){
        return Resource2Functor::getKey(rp.getResource2());
    }
    
    
    template<typename Resource1Functor, typename ResourceProductConsidered>
    bool ResourceProductFeasibilityFunctor_Resource1<Resource1Functor, ResourceProductConsidered>::getKey(const ResourceProductConsidered & rp){
        return Resource1Functor::getKey(rp.getResource1());
    }
    
    template<typename Resource2Functor, typename ResourceProductConsidered>
    bool ResourceProductFeasibilityFunctor_Resource2<Resource2Functor, ResourceProductConsidered>::getKey(const ResourceProductConsidered & rp){
        return Resource2Functor::getKey(rp.getResource2());
    }
    
    template<typename Resource1Functor, typename Resource2Functor, typename ResourceProductConsidered>
    double ResourceProductDistanceFunctor<Resource1Functor, Resource2Functor, ResourceProductConsidered>::getDistance(const ResourceProductConsidered & r1, const ResourceProductConsidered & r2){
        return Resource1Functor::getDistance(r1.getResource1(), r2.getResource1()) + Resource2Functor::getDistance(r1.getResource2(), r2.getResource2());
    }
}