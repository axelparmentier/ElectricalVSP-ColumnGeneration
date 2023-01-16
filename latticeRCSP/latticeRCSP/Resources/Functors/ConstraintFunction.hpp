//
//  constraintFunction.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 21/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

//#include "ConstraintFunction.h"

namespace latticeRCSP {
template <typename resource>
bool ConstraintFunction<resource>::getKey(const resource & r){
    return r.isResourceConstraintSatisfied();
}
    
    template <typename resource>
    bool NoConstraintFunction<resource>::getKey(const resource & r){
        return true;
    }
    
}