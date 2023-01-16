//
//  Path.hpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 02/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "../Resources/ResourceFactory.h"

namespace latticeRCSP {
    template <typename Resource>
    bool Path<Resource>::isPathReverse = false;
    
    template <typename Resource>
    void Path<Resource>::reversePath(){
        isPathReverse = !isPathReverse;
    }
    
    template <typename Resource>
    Path<Resource>::Path(bool isReverse, int vertex){
        pathID = "";
        pathResource = ResourceFactory<Resource>::getStaticNewResource(isReverse);
        
        originVertex = vertex;
        destinationVertex = vertex;
        
        isPathReverse = isReverse;
        
    }
    
    template <typename Resource>
    Path<Resource>::Path(){
        pathID = "non initialized path" ;
    }
    
//    template <typename Resource>
//    Path<Resource>::Path(const Path & original){
//        pathID = original.getID();
//        pathResource = original.getResource();
//    }
    
    template <typename Resource>
    Resource const & Path<Resource>::getResource() const{
        return pathResource;
    }
    
    template <typename Resource>
    bool Path<Resource>::isReverse() {
        return isPathReverse;
    }
    
    template<typename Resource>
    string Path<Resource>::getID() const {
        return pathID;
    }
    
    template <typename Resource>
    int Path<Resource>::getOrigin_neverReverse() const{
        return originVertex;
    }
    
    template<typename Resource >
    void Path<Resource>::setOrigin_neverReverse(int o){
        originVertex = o;
    }
    
    template<typename Resource >
    void Path<Resource>::setDestination_neverReverse(int d){
        destinationVertex = d;
    }
    
    template <typename Resource>
    int Path<Resource>::getDestination_neverReverse() const{
        return destinationVertex;
    }
    
    template <typename Resource>
    int Path<Resource>::getDestination() const{
        if (isReverse()) {
            return originVertex;
        } else return destinationVertex;
    }
    
    template <typename Resource>
    void Path<Resource>::expand(const Path<Resource> & p) {
#ifdef DEBUG_TESTS
        if (!isPathContiguous(p)) throw "Paths expanded are not contiguous";
#endif
        if (isPathReverse) {
            Resource newResource(p.getResource());
            newResource.expand(pathResource);
            pathResource = newResource;
        	pathID = p.getID() + pathID;
            originVertex = p.getOrigin_neverReverse();
        } else {
        	pathResource.expand(p.getResource());
            pathID += p.getID();
            destinationVertex = p.getDestination_neverReverse();
        }
    }
    
    template <typename Resource>
    void Path<Resource>::expand(const Arc<Resource> & newArc){
#ifdef DEBUG_TESTS
        if (!isPathContiguous(newArc)) throw "Paths expanded are not contiguous";
#endif
        if (isPathReverse) {
            Resource newResource = Resource();
            newResource.expand(newArc.getResource());
            newResource.expand(pathResource);
            pathResource = newResource;
        	pathID = newArc.getArcContext()->getID() + pathID;
            originVertex = newArc.getOrigin();
        } else {
        	pathResource.expand(newArc.getResource());
            pathID +=  newArc.getArcContext()->getID();
            destinationVertex = newArc.getDestination();
        }
    }

    template <typename Resource>
    bool Path<Resource>::isPathContiguous(const Path<Resource> & p) const {
        if (isPathReverse) return originVertex == p.getDestination_neverReverse();
        else return destinationVertex == p.getOrigin_neverReverse();
    }
    
    template <typename Resource>
    bool Path<Resource>::isPathContiguous(const Arc<Resource> & newArc) const {
        if (isPathReverse) return originVertex == newArc.getDestination();
        else return destinationVertex == newArc.getOrigin();
    }
    
    template <typename Resource>
    bool Path<Resource>::testIfArcBelongsToPath(const Arc<Resource> & arc) const{
        return (pathID.find(arc.getArcContext()->getID()) != string::npos);
    }
}
