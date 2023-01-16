//
//  InstanceAnyResourceReader.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 22/09/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#ifndef InstanceAnyResourceReader_hpp
#define InstanceAnyResourceReader_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <set>

#include "ArcReader.h"
#include "../Context/ContextGraph.h"
#include "../ReaderAndBuilder/ContextBuilder.h"


using namespace std;

namespace latticeRCSP {
    
    template <typename Resource>
    class SetOfArcsWithResource{
        vector<string> arcsInSet_m;
        Resource resource_m;
        
    public:
        SetOfArcsWithResource();
        SetOfArcsWithResource(ifstream &);
        SetOfArcsWithResource(stringstream &);
        
        vector<string> * getArcsInSet();
        Resource getResource();

    };
    
    template <typename Resource>
    class InstanceAnyResourceReader {
        vector<ArcReader> arcs_m;
        map<string, NewArcContextInformation<Resource> > arcResources_m;
        vector<SetOfArcsWithResource<Resource> > setsOfArcWithResource_m;
        vector<int> origins_m;
        vector<int> destination_m;
        
    public:
        InstanceAnyResourceReader(string file);
        ContextGraph<Resource> * getContextGraph();
        
    protected:
        void computeArcResources();

    };
}


#include "InstanceAnyResourceReader.hpp"

#endif /* InstanceAnyResourceReader_h */
