//
//  ResourceDouble.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceDouble__
#define __latticeRCSP_v2__ResourceDouble__

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>    // std::max

#ifndef EPS
#define EPS 1e-6
#endif

using namespace std;

namespace latticeRCSP {
    class ResourceDouble{
        double value;
    public:
        ResourceDouble();
        ResourceDouble(double const &);
        ResourceDouble(stringstream &);
        
        //---------------------------------------------------
        // Mandatory methods to be used in the resource framework
        //---------------------------------------------------
        // Monoid element ------------
        void expand(const ResourceDouble &);
        
        // Lattice Element -----------
        void meet(const ResourceDouble &);
        void join(const ResourceDouble &);
        
        double distance(const ResourceDouble &) const;
        
        bool isLeq(const ResourceDouble &) const;
        bool isGeq(const ResourceDouble &) const;
        
        // Complete Lattice--
        void setToNeutral();
        void setToSupremum();
        void setToInfimum();
        
        //---------------------------------------------
        // Getters
        //---------------------------------------------
        double getValue() const;
    };
    
    class ResourceDoubleValue{
    public:
        static double getKey(ResourceDouble const &);
    };
}


#endif /* defined(__latticeRCSP_v2__ResourceDouble__) */
