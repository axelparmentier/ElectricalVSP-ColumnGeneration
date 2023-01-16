//
//  NonDominatedPathSet.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__NonDominatedPathSet__
#define __latticeRCSP_v2__NonDominatedPathSet__

#include <stdio.h>
#include <list>

#include "Path.h"

using namespace std;
namespace latticeRCSP {
    template<typename Resource>
    class NonDominatedPathSet {
        list<Path<Resource> > content;
        
    public:
        NonDominatedPathSet();
        
        bool insertIfNonDominated(Path<Resource> const &, int & changeInSize);
        bool insertIfNonDominated(Path<Resource> const &);
    };
}



#include "NonDominatedPathSet.hpp"

#endif /* defined(__latticeRCSP_v2__NonDominatedPathSet__) */
