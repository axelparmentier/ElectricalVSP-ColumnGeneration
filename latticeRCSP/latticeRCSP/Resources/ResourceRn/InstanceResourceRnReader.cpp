////
////  InstanceResourceRnReader.cpp
////  RCSP
////
////  Created by Axel Parmentier on 15/09/2014.
////  Copyright (c) 2014 Axel Parmentier. All rights reserved.
////
//
//#include "InstanceResourceRnReader.h"
//
//
//InstanceResourceRnReader::InstanceResourceRnReader(string filename){
//
//    ifstream read(filename.c_str());
//    if (read.is_open()) {
//
//        // Find the description of the instance (line begins with)
//        //bool descriptionFound = false;
//        string line;
//
//        // origin and destination
//        bool odFound = false;
//        read.clear();
//        read.seekg(0,ios::beg); // back to the beginning of the file
//        while (getline(read, line) && !odFound) {
//            stringstream l(line);
//            char lineType;
//            l >> lineType;
//            if (lineType == 'q') {
//                string dust;
//                l >> isOriginDestinationSet;
//                odFound = true;
//                if (isOriginDestinationSet) {
//                    l >> dust >> origin >> dust >> destination;
//                }
//            }
//        }
//        if (!odFound) isOriginDestinationSet = false;
//
//
//
//        // Number of resources
//        bool nrFound = false;
//        int nResources = -1;
//        read.clear();
//        read.seekg(0,ios::beg); // back to the beginning of the file
//        while (getline(read, line) && !nrFound) {
//            stringstream l(line);
//            char lineType;
//            l >> lineType;
//            if (lineType == 'r') {
//                string dust;
//                l >> dust >> nResources;
//                nrFound = true;
//            }
//        }
//        if (!nrFound) throw "Number of resources is not set";
//        // Read the arcs
//        read.clear();
//        read.seekg(0,ios::beg); // back to the beginning of the file
//        while (getline(read, line)) {
//            stringstream l(line);
//            char lineType;
//            l >> lineType;
//            if (lineType == 'a'){
//                // read the arc and the arc weight
//                pair<int,int> currentArc = pair<int,int>(-1,-1);
//                l >> currentArc.first >> currentArc.second;
//
//                string dust;
//                double currentCost = -1;
//                l >> dust >> currentCost >> dust;
//
//                vector<double> res(nResources+1,-1);
//                res[0] = currentCost;
//                for (int i = 0; i<nResources; i++) {
//                    l >> res[i+1];
//                }
//
//                ResourceRn r(res,0,0);
//
//                Arcs.push_back(currentArc);
//                arcResources.push_back(r);
//            }
//        }
//
//        read.close();
//
//
//    }
//    else throw "Unable to open " + filename;
//}
//
//list<pair<int, int> > & InstanceResourceRnReader::getArcs() {
//    return Arcs;
//}
//
//list<ResourceRn> & InstanceResourceRnReader::getResource() {
//    return arcResources;
//}
//
//int InstanceResourceRnReader::getDestination() const {
//    return destination;
//}
//
//int InstanceResourceRnReader::getOrigin() const{
//    return origin;
//}
//
//bool InstanceResourceRnReader::getIsOriginDestinationSet() const{
//    return isOriginDestinationSet;
//}
//
//
