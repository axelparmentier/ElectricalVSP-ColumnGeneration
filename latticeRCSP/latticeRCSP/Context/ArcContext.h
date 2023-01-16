//
//  ArcContext.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ArcContext__
#define __latticeRCSP_v2__ArcContext__

#include <stdio.h>
#include <string>

using namespace std;


namespace latticeRCSP {
    template <typename Resource>
    class ArcContext {
    private:
        string arcID;
        Resource arcResource;
        
        
    public:
        
        ArcContext(string, const Resource & r);
        string getID() const;
        void setID(string);
        Resource getResource() const;
        Resource * getPtrResource();
        
        void setResource(Resource const &);
        void setResourceToSupremum();
        void setResourceToNeutralElement();
        void setResourceReducedCost(double);
        
        
        
    };
}

#include "ArcContext.hpp"

#endif /* defined(__latticeRCSP_v2__ArcContext__) */
