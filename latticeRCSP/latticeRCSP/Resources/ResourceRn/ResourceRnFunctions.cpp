//
//  ResourceRnFunctions.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 24/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ResourceRnFunctions.h"

namespace latticeRCSP {
    //---------------------------------------------------------------
    int ResourceRnIndexCostFunction::index = 0;
    
    void ResourceRnIndexCostFunction::setIndex(int i){
        index = i;
    }
    
    double ResourceRnIndexCostFunction::getKey(const ResourceRn & r){
        return r.getResourceValue(index);
    }
    
    
    
    //---------------------------------------------------------------
    int ResourceRnIndexConstraintsFunctionParameters::index = 0;
    double ResourceRnIndexConstraintsFunctionParameters::value = INFINITY;
    
    void ResourceRnIndexConstraintsFunctionParameters::setIndex(int i){
        index = i;
    }
    
    void ResourceRnIndexConstraintsFunctionParameters::setValue(double v){
        value = v;
    }
    
    bool ResourceRnIndexConstraintFeasibilityFunction::getKey(const ResourceRn & r){
        return (r.getResourceValue(index) < value + EPS);
    }
    
    double ResourceRnIndexConstraintHeuristicFunction::getKey(const ResourceRn & r){
        return r.getResourceValue(index);
    }
}