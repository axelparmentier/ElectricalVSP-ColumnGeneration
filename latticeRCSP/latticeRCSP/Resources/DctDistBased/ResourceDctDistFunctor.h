//
//  ResourceDctDistFunctor.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceDctDistFunctor__
#define __latticeRCSP_v2__ResourceDctDistFunctor__

#include <stdio.h>
#include "ResourceDctDist.h"
#include "../../Tools/DctDist/DctDistRiskMeasures.h"

namespace latticeRCSP {
    template<typename DctDistFunctor>
    class ResourceDctDistFunctor{
    public:
        static double getKey(const
                             ResourceDctDist &);
    };
    
    typedef ResourceDctDistFunctor<DctDistCVaR> ResourceDctDistCVaR;
    typedef ResourceDctDistFunctor<DctDistExpectation> ResourceDctDistExpectation;
    typedef ResourceDctDistFunctor<DctDistProbaGreaterThanTau> ResourceDctDistProbaGreaterThanTau;
    
    template<typename DctDistFunctor>
    class ResourceDctDistFeasibilityFunctor{
        static double threshold;
    public:
        static bool getKey(ResourceDctDist const &);
        static void setThreshold(double);
        static double getThreshold();
    };
    
    typedef ResourceDctDistFeasibilityFunctor<DctDistCVaR> ResourceDctDistFeasibilityCVaR;
    typedef ResourceDctDistFeasibilityFunctor<DctDistExpectation> ResourceDctDistFeasibilityExpectation;
    typedef ResourceDctDistFeasibilityFunctor<DctDistProbaGreaterThanTau> ResourceDctDistFeasibilityProbaGreaterThanTau;
}


#include "ResourceDctDistFunctor.hpp"

#endif /* defined(__latticeRCSP_v2__ResourceDctDistFunctor__) */
