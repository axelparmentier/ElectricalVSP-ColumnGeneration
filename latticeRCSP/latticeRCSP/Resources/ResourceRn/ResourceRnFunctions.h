//
//  ResourceRnFunctions.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 24/11/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__ResourceRnFunctions__
#define __latticeRCSP__ResourceRnFunctions__

#include <stdio.h>
#include "ResourceRn.h"

namespace latticeRCSP {
    
    //------------------------------------------
    // Single index functions
    //------------------------------------------

    
    // Single resource -------------------------
    // Cost
    class ResourceRnIndexCostFunction{
        static int index;
        
    public:
        static void setIndex(int);
        static double getKey(const ResourceRn &);
    };
    
    
    // Constraint ------------------------------
    class ResourceRnIndexConstraintsFunctionParameters{
    protected:
        static int index;
        static double value;
    public:
        static void setIndex(int);
        static void setValue(double);
    };
    
    class ResourceRnIndexConstraintFeasibilityFunction : public ResourceRnIndexConstraintsFunctionParameters{
    public:
        static bool getKey(const ResourceRn &);
    };
    
    class ResourceRnIndexConstraintHeuristicFunction : public ResourceRnIndexConstraintsFunctionParameters {
    public:
        static double getKey(const ResourceRn &);
    };
    
}

//#include "ResourceRnFunctions.hpp"

#endif /* defined(__latticeRCSP__ResourceRnFunctions__) */
