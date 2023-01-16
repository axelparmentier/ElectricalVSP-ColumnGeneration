//
//  costFunction.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 18/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "CostFunction.h"

namespace latticeRCSP {
    template <typename resource>
    double const * costFunction<resource>::constraintKey = 0;
    
    template <typename resource>
    double costFunction<resource>::getKey(const resource & r){
        return r.getCost();
    }
    
    template <typename resource>
    double costFunction<resource>::getConstraintKey(){
        return *constraintKey;
    }
    
    template <typename resource>
    void costFunction<resource>::setConstraintKey(double const * key){
        constraintKey = key;
    }
}
