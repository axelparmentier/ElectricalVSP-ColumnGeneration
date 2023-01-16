//
//  ResourceDouble.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceDouble.h"

namespace latticeRCSP {
    //-------------------------------------------------------------------------
    // ResourceDouble
    //-------------------------------------------------------------------------
    
    ResourceDouble::ResourceDouble(){
        value = 0;
    }
    
    ResourceDouble::ResourceDouble(double const & value0){
        value = value0;
    }
    
    ResourceDouble::ResourceDouble(stringstream & read){
        string dustbin;
        read >> dustbin >> value;
    }
    
    //---------------------------------------------------
    // Mandatory methods to be used in the resource framework
    //---------------------------------------------
    
    void ResourceDouble::expand(const ResourceDouble &r){
        value += r.getValue();
    }
    
    void ResourceDouble::meet(const ResourceDouble &r){
        value = min(value,r.getValue());
    }
    
    void ResourceDouble::join(const latticeRCSP::ResourceDouble & r){
        value = max(value, r.getValue());
    }
    
    double ResourceDouble::distance(const latticeRCSP::ResourceDouble & r) const{
        return fabs(value - r.getValue());
    }
    
    bool ResourceDouble::isLeq(const latticeRCSP::ResourceDouble & r) const{
        return value < r.getValue() + EPS;
    }
    
    bool ResourceDouble::isGeq(const latticeRCSP::ResourceDouble & r) const{
        return value > r.getValue()- EPS;
    }
    
    void ResourceDouble::setToNeutral(){
        value = 0;
    }
    
    void ResourceDouble::setToSupremum(){
        value = INFINITY;
    }
    
    void ResourceDouble::setToInfimum(){
        value = - INFINITY;
    }
    
    //---------------------------------------------
    // Getters
    //---------------------------------------------
    double ResourceDouble::getValue() const{
        return value;
    }
    
    //-------------------------------------------------------------------------
    // Resource Double Value functor
    //-------------------------------------------------------------------------
    double ResourceDoubleValue::getKey(const latticeRCSP::ResourceDouble & r){
        return r.getValue();
    }
}