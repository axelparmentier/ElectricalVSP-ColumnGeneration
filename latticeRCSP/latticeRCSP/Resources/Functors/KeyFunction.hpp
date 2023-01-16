//
//  keyFunction.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 18/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "KeyFunction.h"

namespace latticeRCSP {
    template <typename resource>
    double keyFunction<resource>::getKey(const resource & r){
        return r.getKey();
    }
}