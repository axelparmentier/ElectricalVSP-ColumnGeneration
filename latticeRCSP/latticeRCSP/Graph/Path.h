//
//  Path.h
//  latticeRCSP
//
//  Created by Axel Parmentier on 02/10/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP__ResourcePath__
#define __latticeRCSP__ResourcePath__

#include <iostream>
#include <list>
#include <math.h>

#include "Arc.h"


// Enable to choose if we wan string or list of arcs (or both) in Paths
//#define PathS_HAVE_STRING


using namespace std;
namespace latticeRCSP {
    //-------------------------------------------------
    // Resource Path is the class from which all Paths should inherit
    //
    // It defines all the functions that are required by the algortihms
    //-------------------------------------------------
    
    template <typename Resource>
    class Path {
    protected:
        //    static double lastKey;
        
        string pathID;
        Resource pathResource;
        
        int originVertex;
        int destinationVertex;
        
        static bool isPathReverse; // For ressource expansion
        
    public:
        Path(bool isReverse, int vertex);
        Path();
//        Path(const Path &);
        
        static void reversePath();
        static bool isReverse();

        
        //---------------------------------------------------
        // Getters
        //---------------------------------------------------
        string getID() const;
        Resource const & getResource() const;
        int getDestination() const;
        
        int getDestination_neverReverse() const;
        int getOrigin_neverReverse() const;
        void setOrigin_neverReverse(int);
        void setDestination_neverReverse(int);
        
        
        //---------------------------------------------------
        // Expansion operation :
        //---------------------------------------------------
        void expand(const Path<Resource> &);
        void expand(const Arc<Resource> &);
    
        bool isPathContiguous(const Path<Resource> &) const;
        bool isPathContiguous(const Arc<Resource> &) const;
        
        //---------------------------------------------------
        // Elementary test
        //---------------------------------------------------
        bool testIfArcBelongsToPath(const Arc<Resource> &) const;

        //---------------------------------------------------
        // Mandatory methods to be used in the Resource framework
        //---------------------------------------------------
        // Lattice Element -----------
        /*
        void meet(const Resource &);
        void join(const ResourceAF);
        
        bool isLeq(const Path<Resource> &) const;
        bool isGeq(const Path<Resource> &) const;
        bool isLeq(const Resource &) const;
        bool isGeq(const Resource &) const;
        
        const Resource * getResource() const;
        double getKey() const;
        double getCost() const;
        
        static double getConstraintKey() ;
        static double getConstraintCost() ;
        
        // Resource constraint -------
        bool isResourceConstraintSatisfied() const;
         */
        
    };
    
}

#include "Path.hpp"

//double Path<Resource>::lastKey = INFINITY;



#endif /* defined(__latticeRCSP__ResourcePath__) */
