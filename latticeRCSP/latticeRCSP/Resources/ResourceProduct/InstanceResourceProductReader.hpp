//
//  InstanceResourceProductReader.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 13/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "InstanceResourceProductReader.h"

namespace latticeRCSP {
    template<typename Resource1, typename Resource2>
    InstanceResourceProductReader<Resource1, Resource2>::InstanceResourceProductReader(string filename){
        ifstream read(filename.c_str());
        if (read.is_open()) {
            
            // Find the description of the instance (line begins with)
            //bool descriptionFound = false;
            string line;
            
            // origin and destination
            bool odFound = false;
            read.clear();
            read.seekg(0,ios::beg); // back to the beginning of the file
            while (getline(read, line) && !odFound) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == 'q') {
                    string dust;
                    l >> isOriginDestinationSet;
                    odFound = true;
                    if (isOriginDestinationSet) {
                        l >> dust >> origin >> dust >> destination;
                    }
                }
            }
            if (!odFound) isOriginDestinationSet = false;
            
            

            // Read the arcs
            read.clear();
            read.seekg(0,ios::beg); // back to the beginning of the file
            while (getline(read, line)) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == 'a'){
                    // read the arc and the arc weight
                    pair<int,int> currentArc = pair<int,int>(-1,-1);
                    l >> currentArc.first >> currentArc.second;
                    Resource1 r1(l);
                    Resource2 r2(l);

                    
                    Arcs.push_back(currentArc);
                    arcResources.push_back(ResourceProduct<Resource1,Resource2>(r1,r2));
                }
            }
            
            cout << "close" << endl;
            read.close();
            
            
        }
        else throw "Unable to open " + filename;
    }
    
    
    template<typename Resource1, typename Resource2>
    list<pair<int, int> > & InstanceResourceProductReader<Resource1, Resource2>::getArcs() {
        return Arcs;
    }
    
    template<typename Resource1, typename Resource2>
    list<ResourceProduct<Resource1, Resource2> > & InstanceResourceProductReader<Resource1, Resource2>::getResource() {
        return arcResources;
    }
    
    template<typename Resource1, typename Resource2>
    int InstanceResourceProductReader<Resource1, Resource2>::getDestination() const {
        return destination;
    }
    
    template<typename Resource1, typename Resource2>
    int InstanceResourceProductReader<Resource1, Resource2>::getOrigin() const{
        return origin;
    }
    
    template<typename Resource1, typename Resource2>
    bool InstanceResourceProductReader<Resource1, Resource2>::getIsOriginDestinationSet() const{
        return isOriginDestinationSet;
    }
}
