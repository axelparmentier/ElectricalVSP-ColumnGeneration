//
//  ResourceDctDist.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceDctDist__
#define __latticeRCSP_v2__ResourceDctDist__

#include <stdio.h>
#include <sstream>

#include "../../Tools/DctDist/DctDist.h"

using namespace ToolsAxel;

namespace latticeRCSP {
    class ResourceDctDist {
        bool isSupremum;
        DctDist dist;
        
        static double supremumDistance;
        
    public:
        ResourceDctDist();
        ResourceDctDist(DctDist &);
        ResourceDctDist(stringstream &);
        
        //---------------------------------------------------
        // Mandatory methods to be used in the resource framework
        //---------------------------------------------------
        // Monoid element ------------
        void expand(const ResourceDctDist &);
        
        // Lattice Element -----------
        void meet(const ResourceDctDist &);
        void join(const ResourceDctDist &);
        
        double distance(const ResourceDctDist &) const;
        
        bool isLeq(const ResourceDctDist &) const;
        bool isGeq(const ResourceDctDist &) const;
        
        // Complete Lattice--
        void setToNeutral();
        void setToSupremum();
//        void setToInfimum();
        
        
        //---------------------------------------------
        // Getters
        //---------------------------------------------
        DctDist const & getDctDist() const;
        bool getIsSupremum() const;
        
    };
}


#endif /* defined(__latticeRCSP_v2__ResourceDctDist__) */
