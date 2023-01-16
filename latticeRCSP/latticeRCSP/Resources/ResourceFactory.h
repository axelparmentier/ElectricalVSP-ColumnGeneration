//
//  ResourceFactory.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 07/04/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResourceFactory__
#define __latticeRCSP_v2__ResourceFactory__

#include <stdio.h>

namespace latticeRCSP {
    
    template<typename Resource>
    class ResourceFactory {
    public:
        static Resource getStaticNewResource(bool isReverse);
        static Resource * getPtrNewResource(bool isReverse);
    };
    
}

#include "ResourceFactory.hpp"

#endif /* defined(__latticeRCSP_v2__ResourceFactory__) */
