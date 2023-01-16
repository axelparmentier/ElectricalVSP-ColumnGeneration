//
//  keyFunction.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 18/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__keyFunction__
#define __latticeRCSP__keyFunction__

#include <stdio.h>

namespace latticeRCSP {
    template <typename resource>
    class keyFunction{
    public:
        static double getKey(const resource & r);
    };
}

#include "KeyFunction.hpp"


#endif /* defined(__latticeRCSP__keyFunction__) */
