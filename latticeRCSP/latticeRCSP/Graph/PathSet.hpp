//
//  PathSet.cpp
//  latticeRCSP
//
//  Created by Axel Parmentier on 17/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "PathSet.h"

namespace latticeRCSP {
    
    template<typename Resource, typename KeyFunctor>
    PathSet<Resource, KeyFunctor>::PathSet(){
        maxSize = 0;
        nElements = 0;
    }
    
    template<typename Resource, typename KeyFunctor>
    PathSet<Resource, KeyFunctor>::PathSet(unsigned int m){
        maxSize = m;
        nElements = 0;
    }
    
    template<typename Resource, typename KeyFunctor>
    PathSet<Resource, KeyFunctor>::PathSet(const PathSet & original){
        maxSize = original.getMaxSize();
        nElements = 0;
        
        map<string, Path<Resource> * > const * content = original.getSolution();
        for (typename map<string, Path<Resource> * >::const_iterator it  = content->begin();\
             it != content->end(); ++it) {
            Path<Resource> insertedPath = *(it->second);
            insert(insertedPath);
        }
    }
    
    
    template<typename Resource, typename KeyFunctor>
    PathSet<Resource, KeyFunctor>::~PathSet(){
        for (typename map<string, Path<Resource> * >::iterator it = mapString.begin(); it != mapString.end(); ++it) {
            delete it->second;
        }
    }
    
    template<typename Resource, typename KeyFunctor>
    int PathSet<Resource, KeyFunctor>::size() const{
        return nElements;
    }
    
    template<typename Resource, typename KeyFunctor>
    double PathSet<Resource, KeyFunctor>::insert(const Path<Resource> & insertedPath){
        // Insert (and delete) only if needed
        if (nElements > 0) {
            typename multimap<double, typename map<string, Path<Resource> * >::iterator  >::iterator it = mapKey.end();
            --it;
            if (nElements < maxSize ) {
                insertWithoutDeletingTheLastElement(insertedPath);
            } else if (KeyFunctor::getKey(insertedPath.getResource()) < it->first) {
                if (insertWithoutDeletingTheLastElement(insertedPath)) {
                    deleteTheLastElement();
                }
            }
        } else if (maxSize > 0){
            insertWithoutDeletingTheLastElement(insertedPath);
        } else {
            cout << "Insertion in a PathSet with MaxSize 0" << endl;
            return INFINITY;
        }
       
        // Return the worst cost in the path Set
        if (nElements == maxSize) {
            typename multimap<double, typename map<string, Path<Resource> * >::iterator  >::iterator it = mapKey.end();
            it = mapKey.end();
            --it;
            return it->first;
        } else return INFINITY;
    }
    
    template<typename Resource, typename KeyFunctor>
    bool PathSet<Resource, KeyFunctor>::insertWithoutDeletingTheLastElement(const Path<Resource> & insertedPath){
        Path<Resource> * p = new Path<Resource>(insertedPath);
        pair<typename map<string, Path<Resource> * >::iterator, bool> insertResult = mapString.insert(pair<string, Path<Resource>*>(p->getID(),p));
        
        if (insertResult.second) {
            nElements++;
            mapKey.insert(pair<double,typename map<string, Path<Resource> * >::iterator>(KeyFunctor::getKey(p->getResource()),insertResult.first));
        }
        
        return insertResult.second;
    }
    
    template<typename Resource, typename KeyFunctor>
    void PathSet<Resource, KeyFunctor>::deleteTheLastElement(){
        typename multimap<double, typename map<string, Path<Resource> * >::iterator  >::iterator it = mapKey.end();
        --it;
        delete it->second->second;
        //                    cout << "Erase key " << it->first << endl;
        mapString.erase(it->second);
        mapKey.erase(it);
        nElements--;
    }
    
    template<typename Resource, typename KeyFunctor>
    void PathSet<Resource, KeyFunctor>::clear(){
        for (typename map<string, Path<Resource> * >::iterator it = mapString.begin(); it != mapString.end(); ++it) {
            delete it->second;
        }
        mapString.clear();
        mapKey.clear();
        nElements = 0;
    }
    
    template<typename Resource, typename KeyFunctor>
    void PathSet<Resource, KeyFunctor>::setMaxSize(unsigned int s){
        maxSize = s;
    }
    
    template<typename Resource, typename KeyFunctor>
    int PathSet<Resource, KeyFunctor>::getMaxSize() const{
        return maxSize;
    }

    template<typename Resource, typename KeyFunctor>
    map<string, Path<Resource> * > const * PathSet<Resource, KeyFunctor>::getSolution() const{
        return & mapString;
    }
    
    template<typename Resource, typename KeyFunctor>
    Path<Resource> *  PathSet<Resource, KeyFunctor>::getMinimumKeySolution(){
        if (nElements > 0) {
            return mapKey.begin()->second->second;
        } else {
            cout << "getting minimum key solution of an empty pathSet" << endl;
            throw "getting minimum key solution of an empty pathSet" ;
        }
        
    }

    template<typename Resource, typename KeyFunctor>
    list<string>  PathSet<Resource, KeyFunctor>::getSolutionString() const{
        list<string> solution;
        for (typename map<string, Path<Resource> * >::const_iterator it = mapString.begin(); it != mapString.end() ; ++it){
            solution.push_back(it->first);
        }
        return solution;
    }


}