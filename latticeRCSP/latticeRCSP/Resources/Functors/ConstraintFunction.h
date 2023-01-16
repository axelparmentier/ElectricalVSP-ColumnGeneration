//
//  constraintFunction.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 21/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__constraintFunction__
#define __latticeRCSP__constraintFunction__

#include <stdio.h>

namespace latticeRCSP {
    template <typename resource>
    class ConstraintFunction{
    public:
        static bool getKey(const resource & r);
    };
    
    template <typename resource>
    class NoConstraintFunction{
    public:
        static bool getKey(const resource & r);
    };
}

#include "ConstraintFunction.hpp"

#endif /* defined(__latticeRCSP__constraintFunction__) */
