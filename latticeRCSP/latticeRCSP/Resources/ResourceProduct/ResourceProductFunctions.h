//
//  ResourceProductFunctions.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceProductFunctions__
#define __latticeRCSP_v2__ResourceProductFunctions__

#include <stdio.h>

namespace latticeRCSP {
    template<typename Resource1Functor, typename ResourceProductConsidered>
    class ResourceProductFunctor_Resource1{
    public:
        static double getKey(const ResourceProductConsidered &);
    };
    
    template<typename Resource2Functor, typename ResourceProductConsidered>
    class ResourceProductFunctor_Resource2{
    public:
        static double getKey(const ResourceProductConsidered &);
    };
    
    
    template<typename Resource2Functor, typename ResourceProductConsidered>
    class ResourceProductFeasibilityFunctor_Resource1{
    public:
        static bool getKey(const ResourceProductConsidered &);
    };
    
    template<typename Resource2Functor, typename ResourceProductConsidered>
    class ResourceProductFeasibilityFunctor_Resource2{
    public:
        static bool getKey(const ResourceProductConsidered &);
    };
    
    template<typename Resource1Functor, typename Resource2Functor, typename ResourceProductConsidered>
    class ResourceProductDistanceFunctor {
    public:
        static double getDistance(ResourceProductConsidered const &, ResourceProductConsidered const &);
    };
    
}

#include "ResourceProductFunctions.hpp"

#endif /* defined(__latticeRCSP_v2__ResourceProductFunctions__) */
