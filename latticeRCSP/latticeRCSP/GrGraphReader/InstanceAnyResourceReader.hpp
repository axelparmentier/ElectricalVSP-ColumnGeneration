//
//  InstanceAnyResourceReader.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 22/09/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#include "InstanceAnyResourceReader.h"
#include "../Tools/FileReader/VectorReaderAndWriter.h"


namespace latticeRCSP {
    
    template<typename Resource>
    SetOfArcsWithResource<Resource>::SetOfArcsWithResource(){
        resource_m = Resource();
        arcsInSet_m = vector<string>(0);
    }
    
    template<typename Resource>
    SetOfArcsWithResource<Resource>::SetOfArcsWithResource(ifstream & read){
        resource_m = Resource(read);
        string dust;
        int nElements;
        read >> dust >> nElements >> dust;
        arcsInSet_m = vector<string>(nElements);
        for (vector<string>::iterator it = arcsInSet_m.begin(); it != arcsInSet_m.end(); ++it){
            read >> *it;
        }
    }
    
    template<typename Resource>
    SetOfArcsWithResource<Resource>::SetOfArcsWithResource(stringstream & read){
        resource_m = Resource(read);
        string dust;
        int nElements;
        read >> dust >> nElements >> dust;
        arcsInSet_m = vector<string>(nElements);
        for (vector<string>::iterator it = arcsInSet_m.begin(); it != arcsInSet_m.end(); ++it){
            read >> *it;
        }
    }
    
    template<typename Resource>
    vector<string> * SetOfArcsWithResource<Resource>::getArcsInSet() {
        return & arcsInSet_m;
    }
    
    template<typename Resource>
    Resource SetOfArcsWithResource<Resource>::getResource() {
        return resource_m;
    }
    
    template<typename Resource>
    InstanceAnyResourceReader<Resource>::InstanceAnyResourceReader(string file){
        arcs_m = VectorReaderAndWriter_ClassElements<ArcReader >::readFile('a',file);
        setsOfArcWithResource_m = VectorReaderAndWriter_ClassElements<SetOfArcsWithResource<Resource> >::readFile('s',file);
//        origins_m = VectorReaderAndWriter<int>::readFile('o',file);
//        destination_m = VectorReaderAndWriter<int>::readFile('d',file);
        ifstream read(file.c_str());
        bool odFound = false;
       if (read.is_open()) {

           // Find the description of the instance (line begins with)
           //bool descriptionFound = false;
           string line;

           // origin and destination
           read.clear();
           read.seekg(0,ios::beg); // back to the beginning of the file
           while (getline(read, line) && !odFound) {
               stringstream l(line);
               char lineType;
               l >> lineType;
               if (lineType == 'q') {
                   string dust;
                   l >> dust;
                   odFound = true;
                   int origin, destination;
                    l >> dust >> origin >> dust >> destination;
                    origins_m = vector<int>(1,origin);
                    destination_m = vector<int>(1,destination);
               }
           }
       }
    if (!odFound) throw;
    }
    
    template<typename Resource>
    void InstanceAnyResourceReader<Resource>::computeArcResources(){
        // Add arcs
        arcResources_m.clear();
        for (vector<ArcReader>::iterator it = arcs_m.begin(); it != arcs_m.end(); ++it) {
            NewArcContextInformation<Resource> info;
            info.originVertexName = to_string(it->getOrigin());
            info.destinationVertexName = to_string(it->getDestination());
            info.arcContextName = it->getID();
            info.arcContextID = info.arcContextName;
            info.arcContextResource = Resource();
            arcResources_m.insert(pair<string, NewArcContextInformation<Resource> >(info.arcContextID,info));
        }
        
        // Sum resources
        for (typename vector<SetOfArcsWithResource<Resource> >::iterator setIt = setsOfArcWithResource_m.begin(); setIt != setsOfArcWithResource_m.end(); ++setIt){
            for (vector<string>::iterator vertexIt = setIt->getArcsInSet()->begin(); vertexIt != setIt->getArcsInSet()->end(); ++ vertexIt){
                typename map<string, NewArcContextInformation<Resource> >::iterator arcIt = arcResources_m.find(*vertexIt);
                if (arcIt == arcResources_m.end()) throw;
                arcIt->second.arcContextResource.expand(setIt->getResource());
            }
        }
    }
    
    template<typename Resource>
    ContextGraph<Resource> * InstanceAnyResourceReader<Resource>::getContextGraph(){
        computeArcResources();
        ContextBuilder<Resource> builder = ContextBuilder<Resource>();
        for (typename map<string, NewArcContextInformation<Resource> >::iterator it = arcResources_m.begin(); it != arcResources_m.end(); ++it) {
            builder.addContextArc(it->second);
        }
        
        for (vector<int>::iterator it = origins_m.begin(); it != origins_m.end(); ++it) {
            builder.addSourceVertex(to_string(*it));
        }
        
        for (vector<int>::iterator it = destination_m.begin(); it != destination_m.end(); ++it) {
            builder.addSinkVertex(to_string(*it));
        }
        
        return builder.getContextGraph();
    }
}
