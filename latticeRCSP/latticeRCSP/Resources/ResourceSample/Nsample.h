//
//  Nsample.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/05/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__Nsample__
#define __latticeRCSP_v2__Nsample__

#define EPS 1e-5

#include <stdio.h>
#include <vector>
#include <iostream>

#define NSAMPLE_TRANSFORM
//#define NSAMPLE_PARALLEL_TRANSFORM

using namespace std;

namespace ResourceSample {
    class Nsample {
        size_t nSamples;
        vector<double> samplesValue;
        
    public:
        Nsample();
        Nsample(Nsample const &);
        Nsample(vector<double> &);
        
        
        //---------------------------
        // Sampling linked methods
        //---------------------------
        
#ifndef NSAMPLE_TRANSFORM
#ifndef NSAMPLE_PARALLEL_TRANSFORM
//        void testComparability(Nsample const *) const;
//        void initializeToZeroOfSameSizeIfNotInitialized(Nsample const *);
//        void initializeCopy(Nsample const *);
//        double distance(const Nsample *) const;
#endif
#endif
        
        size_t getNsamples() const;
        vector<double> getSampleValue() const;
        double getSampleI(int) const;
        vector<double>::const_iterator getSampleBeginIterator() const;
        
        void testComparability(Nsample const &) const;
        void initializeToZeroOfSameSizeIfNotInitialized(Nsample const &);
        void initializeCopy(Nsample const &);
        
#ifndef NSAMPLE_TRANSFORM
#ifndef NSAMPLE_PARALLEL_TRANSFORM
        void testComparability(Nsample const *) const;
        void initializeToZeroOfSameSizeIfNotInitialized(Nsample const *);
        void initializeCopy(Nsample const *);
        double distance(const Nsample *) const;
#endif
#endif
     
        
        
        //---------------------------------------------------
        // Mandatory methods to be used in the resource framework
        //---------------------------------------------------
        // Monoid element ------------
        void expand(const Nsample &);
        
        // Lattice Element -----------
        void meet(const Nsample &);
        void join(const Nsample &);
        
        double distance(const Nsample &) const;

        double distanceToNeutral() const;
        
        bool isLeq(const Nsample &) const;
        bool isGeq(const Nsample &) const;
        bool isNonNegative() const;
        bool isNonPositive() const;
        
        // Complete Lattice--
        void setToNeutral();
        void setToPositivePart();
        void setToNegativePart();
        
        
        //---------------------------------------------------
        // Risk Measure
        //---------------------------------------------------
        double expectation() const;
        double probaStrictlyGreaterThanTau(double tau) const;
        double conditionalValueAtRisk(double beta) const;
        
        double weightedDistance(Nsample const &) const;
        

    };
}



#endif /* defined(__latticeRCSP_v2__Nsample__) */
