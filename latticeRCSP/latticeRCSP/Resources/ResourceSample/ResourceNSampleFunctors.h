//
//  ResourceNSampleFunctors.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 07/05/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceNSampleFunctors__
#define __latticeRCSP_v2__ResourceNSampleFunctors__

#include <stdio.h>
#include "ResourceNSample.h"


namespace ResourceSample {
    
    // -------------------------------
    // Functors
    // -------------------------------
    
    class ResourceNSampleCVaR{
        static double beta;
    public:
        static double getKey(const
                             ResourceNSample &);
        static void setBeta(double);
    };
    
    class ResourceNSampleExpectation{
    public:
        static double getKey(const
                             ResourceNSample &);
    };
    
    class ResourceNSampleProbaGreaterThanTau{
        static double tau;
    public:
        static double getKey(const
                             ResourceNSample &);
        static void setTau(double);
    };
    
    // -------------------------------
    // Feasibility functors declarations
    // -------------------------------
    
    template<typename NsampleFunctor>
    class ResourceNSampleFeasibilityFunctor{
        static double threshold;
    public:
        static bool getKey(ResourceNSample const &);
        static void setThreshold(double);
        static double getThreshold();
    };
    
    typedef ResourceNSampleFeasibilityFunctor<ResourceNSampleCVaR> ResourceNSampleFeasibilityCVaR;
    typedef ResourceNSampleFeasibilityFunctor<ResourceNSampleExpectation> ResourceNSampleFeasibilityExpectation;
    typedef ResourceNSampleFeasibilityFunctor<ResourceNSampleProbaGreaterThanTau> ResourceNSampleFeasibilityProbaGreaterThanTau;
    
    // -------------------------------
    // Template feasibility functor implemenation
    // -------------------------------
    
    template<typename NsampleFunctor>
    double ResourceNSampleFeasibilityFunctor<NsampleFunctor>::threshold = 0;
    
    template<typename NsampleFunctor>
    bool ResourceNSampleFeasibilityFunctor<NsampleFunctor>::getKey(const ResourceNSample & r){
        //        cout << "test: "   << NsampleFunctor::getKey(r.getDctDist()) <<" < " << threshold + EPS;
        return (NsampleFunctor::getKey(r) < threshold + EPS);
    }
    
    template<typename NsampleFunctor>
    void ResourceNSampleFeasibilityFunctor<NsampleFunctor>::setThreshold(double d){
        threshold = d;
    }
    
    template<typename NsampleFunctor>
    double ResourceNSampleFeasibilityFunctor<NsampleFunctor>::getThreshold(){
        return threshold;
    }
}

#endif /* defined(__latticeRCSP_v2__ResourceNSampleFunctors__) */
