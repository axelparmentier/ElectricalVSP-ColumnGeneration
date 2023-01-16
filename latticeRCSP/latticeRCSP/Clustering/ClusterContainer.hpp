//
//  ClusterContainer.cpp
//  latticeClustering
//
//  Created by Axel Parmentier on 09/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ClusterContainer.h"


namespace latticeClustering {
    template <typename Resource>
    ClusterContainer<Resource>::ClusterContainer() {
        initialize();
    }
    
    template <typename Resource>
    ClusterContainer<Resource>::ClusterContainer(vector<Resource const *> const & fixedElem){
        initialize();
        
        fixedElements = fixedElem;
//        nElements_all = fixedElem.size();
        
        for (int i = 0; i < fixedElements.size(); i++) {
            fixedElementsMeet.meet(*fixedElements[i]);
        }
        isMeetUpToDate = true;
        
    }
    
    template <typename Resource>
    void ClusterContainer<Resource>::initialize(){
//        nElements_all = 0;
        
        fixedElements = vector<Resource const *>(0);
        
        join = Resource();
        join.setToInfimum();
        meet = Resource();
        meet.setToSupremum();
        
        fixedElementsMeet = Resource();
        fixedElementsMeet.setToSupremum();
        fixedElementsJoin = Resource();
        fixedElementsJoin.setToInfimum();
        
        isMeetUpToDate = true;
        isJoinUpToDate = true;
    }
    
    template <typename Resource>
    void ClusterContainer<Resource>::computeMeet(){
        meet = fixedElementsMeet;
        for (typename map<int,Resource const *>::iterator it = newElements.begin(); it != newElements.end(); ++it) {
            meet.meet(*(it->second));
        }
        isMeetUpToDate = true;
    }
    
    template <typename Resource>
    void ClusterContainer<Resource>::computeJoin(){
        join = fixedElementsJoin;
        for (typename map<int,Resource const *>::iterator it = newElements.begin(); it != newElements.end(); ++it) {
            join.join(*(it->second));
        }
        isJoinUpToDate = true;
    }
    
    
    template <typename Resource>
    void ClusterContainer<Resource>::insertElement(int i,Resource const * e) {
//        nElements_all++;
        newElements.insert(pair<int,Resource const *>(i,e));
        isMeetUpToDate = false;
        isJoinUpToDate = false;
    }
    
    template <typename Resource>
    void  ClusterContainer<Resource>::removeElement(int i){
//        nElements_all--;
        typename map<int,Resource const *>::iterator it = newElements.find(i);
        if (it == newElements.end()) throw "Not found !";
        newElements.erase(it);
        isMeetUpToDate = false;
        isJoinUpToDate = false;
        
    }
    
    template <typename Resource>
    void  ClusterContainer<Resource>::clear(){
        newElements.clear();
        join.setToInfimum();
        meet.setToSupremum();
    }
    
//    template <typename Resource>
//    void ClusterContainer<Resource>::swap(int elementRemoved,int elementAdded, Resource const * elAdded){
//        if (newElements.find(elementRemoved) == newElements.end()) throw "Not found !";
//        newElements.erase(elementRemoved);
//        newElements.insert(pair<int,Resource*>(elementAdded,elAdded));
//        isMeetUpToDate = false;
//        isJoinUpToDate = false;
//    }
    
    template <typename Resource>
    unsigned int ClusterContainer<Resource>::size_allElements() const{
        return fixedElements.size() + newElements.size();
    }
    
    template <typename Resource>
    unsigned int ClusterContainer<Resource>::size_nonFixedElements() const{
        return newElements.size();
    }
    
    template <typename Resource>
    Resource ClusterContainer<Resource>::getJoin(){
        if (!isJoinUpToDate) computeJoin();
        return join;
    }
    
    template <typename Resource>
    Resource ClusterContainer<Resource>::getMeet(){
        if (!isMeetUpToDate) computeMeet();
        return meet;
    }
    
    template<typename Resource>
    double ClusterContainer<Resource>::getSumOfDistancesToMeet(){
        double solution = 0;
        if (! isMeetUpToDate) {
            computeMeet();
        }
        for (int i = 0; i< fixedElements.size(); i++) {
            solution += fixedElements[i]->distance(meet);
        }
        
        for (typename map<int,Resource const *>::iterator it = newElements.begin(); it != newElements.end(); ++it) {
            solution += it->second->distance(meet);
        }
        return solution;
    }
    
    template<typename Resource>
    vector<int> ClusterContainer<Resource>::getNewElements() const {
        vector<int> solution(newElements.size());
        int counter = 0;
        for (typename map<int,Resource const *>::const_iterator it = newElements.begin(); it != newElements.end(); ++ it){
            solution[counter] = it->first;
            counter++;
        }
        return solution;
    }
    
    template<typename Resource>
    typename vector<Resource const *>::const_iterator ClusterContainer<Resource>::getFixedElementsBegin() const{
        return fixedElements.begin();
    }
    
    template<typename Resource>
    typename vector<Resource const *>::const_iterator ClusterContainer<Resource>::getFixedElementsEnd() const{
        return fixedElements.end();
        
    }
    
    template<typename Resource>
    typename map<int, Resource const *>::const_iterator ClusterContainer<Resource>::getNewElementsBegin() const{
        return newElements.begin();
        
    }
    
    template<typename Resource>
    typename map<int, Resource const *>::const_iterator ClusterContainer<Resource>::getNewElementsEnd() const{
        return newElements.end();
        
    }
}




