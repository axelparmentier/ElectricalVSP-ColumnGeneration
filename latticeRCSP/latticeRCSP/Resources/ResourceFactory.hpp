//
//  ResourceFactory.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 07/04/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

namespace latticeRCSP {
    template<typename Resource>
    Resource * ResourceFactory<Resource>::getPtrNewResource(bool isReverse){
        return new Resource();
    }
    
    template<typename Resource>
    Resource ResourceFactory<Resource>::getStaticNewResource(bool isReverse){
        return Resource();
    }
}