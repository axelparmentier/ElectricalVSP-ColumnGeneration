//
//  Arc.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "Arc.h"

namespace latticeRCSP {
    template<typename Resource>
    Arc<Resource>::Arc(){
        pArcContext = 0;
        origin = -1;
        destination = -1;
    }
    
    template<typename Resource>
    Arc<Resource>::Arc(ArcContext<Resource> const * pArcCont, int ori, int dest, bool isReverse){
        pArcContext = pArcCont;
        if (!isReverse){
            origin = ori;
            destination = dest;
        } else {
            origin = dest;
            destination = ori;
        }
        
    }
    
    template<typename Resource>
	void Arc<Resource>::setDestination(int d, bool isReverse){
    	if (isReverse) origin = d;
    	else destination = d;
    }

    template<typename Resource>
    Resource Arc<Resource>::getResource() const{
        return pArcContext->getResource();
    }
    
    template<typename Resource>
    int Arc<Resource>::getOrigin() const{
        return origin;
    }
    
    template<typename Resource>
    int Arc<Resource>::getDestination() const{
        return destination;
    }

    template<typename Resource>
    ArcContext<Resource> const * Arc<Resource>::getArcContext() const{
        return pArcContext;
    }
    
    


}
