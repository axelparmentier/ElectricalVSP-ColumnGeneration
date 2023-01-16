//
//  ResourceRn.h
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __RCSP__ResourceRn__
#define __RCSP__ResourceRn__

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <math.h>

//#define DEBUG_TESTS
#define EPS 1e-5

using namespace std;

namespace latticeRCSP{
    
    

class ResourceRn {
protected:
    vector<double> resourceValue;
    
    static int nResources;
    static int costIndex; // If -1 then cost is not set.
//    static int keyIndex;
    
//    static int constraintNResources;
    static vector<double> constraintValue;
    
    
    
public:
    //---------------------------------------------------
    // Mandatory methods to be used in the resource framework
    //---------------------------------------------------
    // Monoid element ------------
    void expand(const ResourceRn &);
    
    // Lattice Element -----------
    void meet(const ResourceRn &);
    void join(const ResourceRn &);
    
    double distance(const ResourceRn &) const;
    
    bool isLeq(const ResourceRn &) const;
    bool isGeq(const ResourceRn &) const;
    
    // Complete Lattice--
    void setToSupremum();
    void setToInfimum();
    void setToNeutral();
    
    
    // Resource element ----------
    double getKey() const;
    double getCost() const;
    
    //---------------------------------------------------
    // Constraint: should be in a specific object -------
    // resource constraint -------
    bool isResourceConstraintSatisfied() const;
    static void setResourceConstraint(vector<double> & r);
    
    // Constraint
//    static double getConstraintCost();
//    static double getConstraintKey();
//    static void setConstraintCost();
//    static double setConstraintKey();
    
    
    //---------------------------------------------------
    // Constructor and initializer
    //---------------------------------------------------
    ResourceRn();
    ResourceRn(stringstream &);
    // virtual ResourceRn* Clone() const;
    // ResourceRn(const ResourceRn &);
    ResourceRn(vector<double> &); // costIndex and KeyIndex initialized to 0
//    ResourceRn(vector<double> &, int costIndex, int keyIndex);
    
    //ResourceRn(int nRandomResources); // Random
    
    //---------------------------------------------------
    // Getters and setters
    //---------------------------------------------------
//    int getCostIndex() const;
//    int getKeyIndex() const;
//    void setCost(double cost);
    static void setCostIndex(int);
    static int getNResources();
    double getResourceValue(int) const;
    const vector<double> & getResourceValue() const;
    

};
    
}

#endif /* defined(__RCSP__resource__) */
