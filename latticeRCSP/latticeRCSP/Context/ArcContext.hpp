//
//  ArcContext.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ArcContext.h"

namespace latticeRCSP {
    template<typename Resource>
    ArcContext<Resource>::ArcContext(string arc_ID, const Resource & r){
        arcID = arc_ID;
        arcResource = r;
    }
    
    template <typename Resource>
    string ArcContext<Resource>::getID() const{
        return arcID;
    }

    template <typename Resource>
    void ArcContext<Resource>::setID(string newID){
        arcID = newID;
    }
    
    template <typename Resource>
    Resource ArcContext<Resource>::getResource() const{
        return arcResource;
    }
    
    template <typename Resource>
    Resource * ArcContext<Resource>::getPtrResource() {
        return & arcResource;
    }
    
    template <typename Resource>
    void ArcContext<Resource>::setResource(Resource const & res) {
       arcResource = res;
    }

    template <typename Resource>
    void ArcContext<Resource>::setResourceToSupremum(){
        arcResource.setToSupremum();
    }
    
    template <typename Resource>
    void ArcContext<Resource>::setResourceToNeutralElement(){
        arcResource.setToNeutral();
    }
    
    template<typename Resource>
    void ArcContext<Resource>::setResourceReducedCost(double redCost){
        arcResource.setReducedCost(redCost);
    }
}
