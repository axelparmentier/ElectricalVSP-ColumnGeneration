//
//  TruncatedLognormalDistribution.hpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#ifndef TruncatedLognormalDistribution_hpp
#define TruncatedLognormalDistribution_hpp

#include <stdio.h>
#include <vector>

using namespace std;

class TruncatedLognormalDistribution {
    double mu;
    double sigma;
    
    static double sqrt2;
    static double discretization;
    static double defaultMu;
    static int maxValueRatioDiscretization; // multiple of discretization
    
public:
//    TruncatedLognormalDistribution(double mu, double sigma, double maxRatioExpectation);
    TruncatedLognormalDistribution(double expectation);
    
    static void setDiscretization(double);
    static void setDefaultMu(double);
    static void setMaxValueRatioDiscretization(int);
    
    vector<double> getCumulativeNonNormalized();
};

#endif /* TruncatedLognormalDistribution_hpp */
