//
//  ResourceDctDistFunctor.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceDctDistFunctor.h"

namespace latticeRCSP {
    
    
    template<typename DctDistFunctor>
    double ResourceDctDistFunctor<DctDistFunctor>::getKey(const
                                                          ResourceDctDist & r){
        if (r.getIsSupremum()) {
            return INFINITY;
        } else return DctDistFunctor::getKey(r.getDctDist());
    }
    
    template<typename DctDistFunctor>
    double ResourceDctDistFeasibilityFunctor<DctDistFunctor>::threshold = 0;
    
    template<typename DctDistFunctor>
    bool ResourceDctDistFeasibilityFunctor<DctDistFunctor>::getKey(const ResourceDctDist & r){
//        cout << "test: "   << DctDistFunctor::getKey(r.getDctDist()) <<" < " << threshold + EPS;
        return (DctDistFunctor::getKey(r.getDctDist()) < threshold + EPS);
    }
                
    template<typename DctDistFunctor>
    void ResourceDctDistFeasibilityFunctor<DctDistFunctor>::setThreshold(double d){
        threshold = d;
    }
    
    template<typename DctDistFunctor>
    double ResourceDctDistFeasibilityFunctor<DctDistFunctor>::getThreshold(){
        return threshold;
    }
}

