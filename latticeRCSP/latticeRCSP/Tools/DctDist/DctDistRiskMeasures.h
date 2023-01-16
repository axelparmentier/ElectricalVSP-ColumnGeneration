//
//  DctDistRiskMeasures.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__DctDistRiskMeasures__
#define __latticeRCSP_v2__DctDistRiskMeasures__

#include <stdio.h>

#include "../../Tools/DctDist/DctDist.h"

using namespace ToolsAxel;

namespace latticeRCSP {
    
    //--------------------------------
    // Time Cost
    //--------------------------------
    // Still not implemented but a time cost can be considered
    
    //--------------------------------
    // CVaR
    //--------------------------------
    
    class DctDistCVaR{
        static double beta;
    public:
        static double getKey(DctDist const &);
        static void setBeta(double);
    };
    
    //--------------------------------
    // P(X =< tau)
    //--------------------------------
    
    class DctDistProbaGreaterThanTau{
        static double tau;
    public:
        static double getKey(DctDist const &);
        static void setTau(int);
    };
    
    //--------------------------------
    // Expectation
    //--------------------------------
    
    class DctDistExpectation{
    public:
        static double getKey(DctDist const &);
    };
}

#endif /* defined(__latticeRCSP_v2__DctDistRiskMeasures__) */
