//
//  ResourceNSample.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 12/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceNSample__
#define __latticeRCSP_v2__ResourceNSample__

#include <stdio.h>

#include "Nsample.h"
namespace ResourceSample {
    class ResourceNSample {
        static double distanceToSupremum;
        static double distanceToInfimum;
        typedef enum{
            INFIMUM,
            NEUTRAL,
            SUPREMUM,
            SAMPLE
        } SampleType;
        
        SampleType type_m;
        Nsample sample_m;
        
        
    public:
        ResourceNSample();
        ResourceNSample(Nsample const &);
        ResourceNSample(ifstream &);
        ResourceNSample(stringstream &);
        
        //---------------------------------------------------
        // Getters and Setters
        //---------------------------------------------------
        SampleType getType() const;
        Nsample const & getNSample() const;
        
        //---------------------------------------------------
        // Mandatory methods to be used in the resource framework
        //---------------------------------------------------
        // Monoid element ------------
        void expand(const ResourceNSample&);
        
        // Lattice Element -----------
        void meet(const ResourceNSample &);
        void join(const ResourceNSample &);
        
        double distance(const ResourceNSample &) const;
        
        bool isLeq(const ResourceNSample &) const;
        bool isGeq(const ResourceNSample &) const;
        
        // Complete Lattice--
        void setToNeutral();
        void setToSupremum();
        void setToInfimum();
        
        //---------------------------------------------------
        // Functors
        //---------------------------------------------------
        double expectation() const;
        double probaStrictlyGreaterThanTau(double tau) const;
        double conditionalValueAtRisk(double beta) const;
        
        
        double weightedDistance(const ResourceNSample &) const;
    };
    
    class WeightedDistanceResourceNSampleFunctor{
    public:
        static double getDistance(const ResourceNSample &, const ResourceNSample &);
    };
}

#endif /* defined(__latticeRCSP_v2__ResourceNSample__) */
