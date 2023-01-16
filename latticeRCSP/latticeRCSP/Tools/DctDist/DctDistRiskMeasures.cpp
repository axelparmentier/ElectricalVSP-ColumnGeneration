//
//  DctDistRiskMeasures.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "DctDistRiskMeasures.h"



namespace latticeRCSP {
    //--------------------------------
    // CVaR
    //--------------------------------
    
    double DctDistCVaR::beta = 0.05;
    
    double DctDistCVaR::getKey(const DctDist & dist){
#ifdef DEBUG_TESTS
        if (!dist.isCumulative()) throw "error";
#endif
        
        double solution = 0;
        //double stop = false;
        vector<double> f = dist.getF();
        int t = f.size();
        while (t>0 && 1 - f[t-1] <= beta) {
            t--;
            solution += t * (f[t] - f[t-1]);
        }
        solution -= ((1 -f[t-1]) - beta)* t;
        
        solution /= beta;
        
        return dist.getMinimumT() + solution;
        
        //------------------
        //Density verstion
        //------------------
//        double solution = 0;
//        double cumul = 0;
//        //double stop = false;
//        vector<double> f = dist.getF();
//        int t = f.size();
//        while (t>0 && cumul <= beta) {
//            t--;
//            solution += t * f[t];
//            cumul += f[t];
//        }
//        solution -= (cumul - beta)* t;
//        
//        solution /= beta;
//        
//        return solution;
    }
    
    void DctDistCVaR::setBeta(double beta0){
        if (beta0 < 0 || beta0 > 1) {
            throw "Beta must be between 0 and 1";
        } else beta = beta0;
    }
    
    
    //--------------------------------
    // P(X =< tau)
    //--------------------------------
    
    double DctDistProbaGreaterThanTau::tau = 50;
    
    double DctDistProbaGreaterThanTau::getKey(const ToolsAxel::DctDist & dist){
#ifdef DEBUG_TESTS
        if (!dist.isCumulative()) throw "error";
#endif
//        cout << "tau" << DctDistProbaGreaterThanTau::tau << endl;
        if (tau < dist.getMinimumT()) {
            return 1;
        }else if (tau < dist.getMinimumT() + dist.getFsize()) {
            return 1 - dist.getF()[tau - dist.getMinimumT()];
        } else return 0;
        
    }
    
    void DctDistProbaGreaterThanTau::setTau(int tau0){
        tau = tau0;
    }
    
    //--------------------------------
    // Expectation
    //--------------------------------
    
    double DctDistExpectation::getKey(const ToolsAxel::DctDist & dist){
#ifdef DEBUG_TESTS
        if (!dist.isCumulative()) throw "error";
#endif
        return dist.expectationCumulative();
    }
    

}