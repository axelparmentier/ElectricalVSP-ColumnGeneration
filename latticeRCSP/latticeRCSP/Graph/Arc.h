//
//  Arc.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__Arc__
#define __latticeRCSP_v2__Arc__

#include <stdio.h>

#include "../Context/ArcContext.h"

namespace latticeRCSP {
    template<typename Resource>
    class Arc {
        ArcContext<Resource> const * pArcContext;
        int origin;
        int destination;
        
    public:
        Arc();
        Arc(ArcContext<Resource> const * pArcCont, int origin, int destination, bool isReverse = false);
        
        int getOrigin() const;
        int getDestination() const;
        void setDestination(int, bool isReverse=false);
        Resource getResource() const;
        ArcContext<Resource> const * getArcContext() const;
        //string getID() const;
        
    };
}

#include "Arc.hpp"

#endif /* defined(__latticeRCSP_v2__Arc__) */
