//
//  ResourceNSampleFunctors.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 07/05/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceNSampleFunctors.h"

namespace ResourceSample {
    // -------------------------------
    // CVaR
    // -------------------------------
    
    double ResourceNSampleCVaR::beta = 0.05;
    
    double ResourceNSampleCVaR::getKey(const ResourceSample::ResourceNSample & r){
        return r.conditionalValueAtRisk(beta);
    }
    
    void ResourceNSampleCVaR::setBeta(double b){
        beta = b;
    }
    
    // -------------------------------
    // Expectation
    // -------------------------------
    
    double ResourceNSampleExpectation::getKey(const ResourceNSample & r){
        return r.expectation();
    }
    
    // -------------------------------
    // Proba greater than tau
    // -------------------------------
    double ResourceNSampleProbaGreaterThanTau::tau = 0;
    
    double ResourceNSampleProbaGreaterThanTau::getKey(const ResourceSample::ResourceNSample & r){
        return r.probaStrictlyGreaterThanTau(tau);
    }
    
    void ResourceNSampleProbaGreaterThanTau::setTau(double t){
        tau = t;
    }

    
}