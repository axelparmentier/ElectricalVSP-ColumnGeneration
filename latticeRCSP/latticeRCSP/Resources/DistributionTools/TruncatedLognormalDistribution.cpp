//
//  TruncatedLognormalDistribution.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#include "TruncatedLognormalDistribution.h"
#include <cmath>

double TruncatedLognormalDistribution::sqrt2 = sqrt(2);
double TruncatedLognormalDistribution::defaultMu = 0;
double TruncatedLognormalDistribution::discretization = 5;
int TruncatedLognormalDistribution::maxValueRatioDiscretization = 18;

TruncatedLognormalDistribution::TruncatedLognormalDistribution(double expectation){
    mu = defaultMu;
    sigma = sqrt(2.* log(expectation) - mu);
}

void TruncatedLognormalDistribution::setDefaultMu(double mu_l){
    defaultMu = mu_l;
}

void TruncatedLognormalDistribution::setDiscretization(double discretization_l){
    discretization = discretization_l;
}

void TruncatedLognormalDistribution::setMaxValueRatioDiscretization(int maxValueRatioDiscretization_l){
    maxValueRatioDiscretization = maxValueRatioDiscretization_l;
}

vector<double> TruncatedLognormalDistribution::getCumulativeNonNormalized(){
    vector<double> cumulativeDistribution(maxValueRatioDiscretization);
    
    for (int t=0; t<cumulativeDistribution.size(); ++t) {
        double x = t + 1;
        cumulativeDistribution[t] = .5 + .5 * erf((log(x) - mu)/(sqrt2 * sigma));
    }
    
    //renormalization
    double a = (1- cumulativeDistribution[maxValueRatioDiscretization - 1]) /(double) (maxValueRatioDiscretization - 1);
    
    //derive
    for (size_t t = cumulativeDistribution.size(); t > 0; --t) {
        cumulativeDistribution[t] -= cumulativeDistribution[t-1];
    }
    
    for (int t=0; t<cumulativeDistribution.size(); ++t) {
        cumulativeDistribution[t] += a;
    }
    
    for (int t=1; t<cumulativeDistribution.size(); ++t) {
        cumulativeDistribution[t] += cumulativeDistribution[t-1];
    }

    return cumulativeDistribution;
}

