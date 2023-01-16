//
//  costFunction.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 18/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__costFunction__
#define __latticeRCSP__costFunction__

#include <stdio.h>

namespace latticeRCSP {
    template <typename resource>
    class costFunction{
        static double const * constraintKey;
    public:
        static double getKey(const resource & r);
        static void setConstraintKey(double const * key);
        static double getConstraintKey();
    };
}


#include "CostFunction.hpp"

#endif /* defined(__latticeRCSP__costFunction__) */
