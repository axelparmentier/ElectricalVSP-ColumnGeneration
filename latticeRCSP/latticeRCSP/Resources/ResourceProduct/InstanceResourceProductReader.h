//
//  InstanceResourceProductReader.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 13/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__InstanceResourceProductReader__
#define __latticeRCSP_v2__InstanceResourceProductReader__

#include <stdio.h>
#include "ResourceProduct.h"

namespace latticeRCSP {
    template<typename Resource1, typename Resource2>
    class InstanceResourceProductReader{
        list<pair<int,int> > Arcs;
        list<ResourceProduct<Resource1, Resource2> > arcResources;
        
        bool isOriginDestinationSet;
        int origin;
        int destination;
        
    public:
        InstanceResourceProductReader(string filename);
        
        //--------------------
        // Getters
        //--------------------
        list<pair<int,int> > & getArcs() ;
        list<ResourceProduct<Resource1, Resource2> > & getResource();
        
        bool getIsOriginDestinationSet() const;
        int getOrigin() const;
        int getDestination() const;
    };
}

#include "InstanceResourceProductReader.hpp"

#endif /* defined(__latticeRCSP_v2__InstanceResourceProductReader__) */
