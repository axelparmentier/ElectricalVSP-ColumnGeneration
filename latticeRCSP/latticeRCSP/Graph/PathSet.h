//
//  PathSet.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 17/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__PathSet__
#define __latticeRCSP__PathSet__

#include <stdio.h>
#include <map>
#include "Path.h"

namespace latticeRCSP {
    
    template<typename resource, typename keyFunctor>
    class PathSet {
    protected:
        unsigned int maxSize;
        unsigned int nElements;
        map<string, Path<resource> * > mapString;
        multimap<double, typename map<string, Path<resource> * >::iterator > mapKey;
        
    public:
        PathSet();
        PathSet(const PathSet &);
        PathSet(unsigned int);
        ~PathSet();
        
        double insert(const Path<resource> &);
        void setMaxSize(unsigned int);
        int getMaxSize() const;
        map<string, Path<resource> * > const * getSolution() const;
        list<string> getSolutionString() const;
        Path<resource> * getMinimumKeySolution();
        int size() const;
        void clear();
        
    private:
        bool insertWithoutDeletingTheLastElement(const Path<resource> &);
        void deleteTheLastElement();
        
    };
    
}

#include "PathSet.hpp"



#endif /* defined(__latticeRCSP__PathSet__) */
