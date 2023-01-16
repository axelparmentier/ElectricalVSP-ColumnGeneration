//
//  LinearCombinationFunction.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__LinearCombinationFunction__
#define __latticeRCSP_v2__LinearCombinationFunction__

#include <stdio.h>

namespace latticeRCSP {
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    class LinearCombinationFunction{
        static double proportionOfFirstFunctor;
        
        static double firstFunctorNormalization;
        static double secondFunctorNormalization;
        
        static double firstFunctorCoefficient;
        static double secondFunctorCoefficient;

        
    public:
        static void setFirstFunctorNormalization(double);
        static void setSecondFunctorNormalization(double);
        static void setProportionOfFirstFunctor(double);
        
        static double getKey(const Resource &);
        
    };
}

#include "LinearCombinationFunction.hpp"

#endif /* defined(__latticeRCSP_v2__LinearCombinationFunction__) */
