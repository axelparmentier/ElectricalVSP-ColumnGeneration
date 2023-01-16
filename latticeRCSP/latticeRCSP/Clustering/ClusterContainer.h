//
//  ClusterContainer.h
//  latticeClustering
//
//  Created by Axel Parmentier on 09/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeClustering__ClusterContainer__
#define __latticeClustering__ClusterContainer__

#include <stdio.h>
#include <map>
#include <vector>


using namespace std;

namespace latticeClustering {
    template <typename Resource>
    class ClusterContainer{
//        unsigned int nElements_all;
    protected:
        
        vector<Resource const *> fixedElements;
        Resource fixedElementsMeet;
        Resource fixedElementsJoin;
        
        map<int,Resource const *> newElements;
        Resource join;
        Resource meet;
        
        bool isMeetUpToDate;
        bool isJoinUpToDate;
        
        void computeMeet();
        void computeJoin();
        
    public:
        ClusterContainer();
        ClusterContainer(vector<Resource const *> const &);
        void initialize();
        
        unsigned int size_allElements() const;
        unsigned int size_nonFixedElements() const;
        
        void insertElement(int, const Resource *);
        void removeElement(int);
//        void swap(int elementRemoved,int elementAdded, const Resource * elAdded);
        void clear();
        
        Resource getJoin();
        Resource getMeet();
        typename vector<Resource const *>::const_iterator getFixedElementsBegin() const;
        typename vector<Resource const *>::const_iterator getFixedElementsEnd() const;
        typename map<int, Resource const *>::const_iterator getNewElementsBegin() const;
        typename map<int, Resource const *>::const_iterator getNewElementsEnd() const;
        
        
        double getSumOfDistancesToMeet();
        vector<int> getNewElements() const;
        
    };
}


#include "ClusterContainer.hpp"

#endif /* defined(__latticeClustering__ClusterContainer__) */
