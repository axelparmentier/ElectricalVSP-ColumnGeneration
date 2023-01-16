//
//  ResourceProduct.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceProduct__
#define __latticeRCSP_v2__ResourceProduct__

#include <stdio.h>



namespace latticeRCSP {
    template<typename Resource1, typename Resource2>
    class ResourceProduct {
        Resource1 resource1_m;
        Resource2 resource2_m;
        
        static double weightInDistanceOfResource1;
        static double weightInDistanceOfResource2;

        
    public:
        ResourceProduct();
        ResourceProduct(const Resource1 &, const Resource2 &);
        
        //---------------------------------------------------
        // Mandatory methods to be used in the resource framework
        //---------------------------------------------------
        // Monoid element ------------
        void expand(const ResourceProduct<Resource1,Resource2> &);
        
        // Lattice Element -----------
        void meet(const ResourceProduct<Resource1,Resource2> &);
        void join(const ResourceProduct<Resource1,Resource2> &);
        
        double distance(const ResourceProduct<Resource1,Resource2> &) const;
        
        bool isLeq(const ResourceProduct<Resource1,Resource2> &) const;
        bool isGeq(const ResourceProduct<Resource1,Resource2> &) const;
        
        // Complete Lattice--
        void setToNeutral();
        void setToSupremum();
        void setToInfimum();
        
        //---------------------------------------------------
        // Getters and setters
        //---------------------------------------------------
        Resource1 const & getResource1() const;
        Resource2 const & getResource2() const;
        
        Resource1 * getPtrResource1() ;
        Resource2 * getPtrResource2() ;
        
        static void setWeightInDistanceOfResource1(double);
        static void setWeightInDistanceOfResource2(double);


    };
}

#include "ResourceProduct.hpp"

#endif /* defined(__latticeRCSP_v2__ResourceProduct__) */
