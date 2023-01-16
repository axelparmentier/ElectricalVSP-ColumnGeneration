//
//  ResourceProduct.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceProduct.h"

namespace latticeRCSP {
    template<typename Resource1, typename Resource2>
    double ResourceProduct<Resource1, Resource2>::weightInDistanceOfResource1 = 1;
    
    template<typename Resource1, typename Resource2>
    double ResourceProduct<Resource1, Resource2>::weightInDistanceOfResource2 = 1;
    
    template<typename Resource1, typename Resource2>
    ResourceProduct<Resource1, Resource2>::ResourceProduct(){
        resource1_m = Resource1();
        resource2_m = Resource2();
    }
    
    //---------------------------------------------------
    // Mandatory methods to be used in the resource framework
    //---------------------------------------------------
    template<typename Resource1, typename Resource2>
    ResourceProduct<Resource1, Resource2>::ResourceProduct(Resource1 const & r1, Resource2 const & r2){
        resource1_m = Resource1(r1);
        resource2_m = Resource2(r2);
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::expand(const ResourceProduct<Resource1, Resource2> & resProd){
        resource1_m.expand(resProd.getResource1());
        resource2_m.expand(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::meet(const ResourceProduct<Resource1, Resource2> & resProd){
        resource1_m.meet(resProd.getResource1());
        resource2_m.meet(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::join(const ResourceProduct<Resource1, Resource2> & resProd){
        resource1_m.join(resProd.getResource1());
        resource2_m.join(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    double ResourceProduct<Resource1, Resource2>::distance(const ResourceProduct<Resource1, Resource2> & resProd) const{
        return weightInDistanceOfResource1 * resource1_m.distance(resProd.getResource1()) \
        + weightInDistanceOfResource2 * resource2_m.distance(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    bool ResourceProduct<Resource1, Resource2>::isLeq(const ResourceProduct<Resource1, Resource2> & resProd) const{
        return resource1_m.isLeq(resProd.getResource1()) \
        &&  resource2_m.isLeq(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    bool ResourceProduct<Resource1, Resource2>::isGeq(const ResourceProduct<Resource1, Resource2> & resProd) const{
        return resource1_m.isGeq(resProd.getResource1()) \
        &&  resource2_m.isGeq(resProd.getResource2());
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::setToNeutral(){
        resource1_m.setToNeutral();
        resource2_m.setToNeutral();
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::setToSupremum(){
        resource1_m.setToSupremum();
        resource2_m.setToSupremum();
    }

    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::setToInfimum(){
        resource1_m.setToInfimum();
        resource2_m.setToInfimum();
    }
    
    //---------------------------------------------------
    // Getters and setters
    //---------------------------------------------------
    template<typename Resource1, typename Resource2>
    Resource1 const & ResourceProduct<Resource1, Resource2>::getResource1() const{
        return resource1_m;
    }
    
    template<typename Resource1, typename Resource2>
    Resource2 const & ResourceProduct<Resource1, Resource2>::getResource2() const{
        return resource2_m;
    }

    template<typename Resource1, typename Resource2>
    Resource1 * ResourceProduct<Resource1, Resource2>::getPtrResource1() {
        return & resource1_m;
    }
    
    template<typename Resource1, typename Resource2>
    Resource2 * ResourceProduct<Resource1, Resource2>::getPtrResource2() {
        return & resource2_m;
    }

    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::setWeightInDistanceOfResource1(double weight){
        weightInDistanceOfResource1 = weight;
    }
    
    template<typename Resource1, typename Resource2>
    void ResourceProduct<Resource1, Resource2>::setWeightInDistanceOfResource2(double weight){
        weightInDistanceOfResource2 = weight;
    }
}