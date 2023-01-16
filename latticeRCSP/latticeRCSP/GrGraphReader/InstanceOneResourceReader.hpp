/*
 * InstanceOneResourceReader.cpp
 *
 *  Created on: 25 juil. 2016
 *      Author: axelparmentier
 */

#include "InstanceOneResourceReader.h"
#include "../Tools/FileReader/VectorReaderAndWriter.h"

namespace latticeRCSP {

template<typename Resource>
int ArcResourceReader<Resource>::current_id = 0;

template<typename Resource>
ArcResourceReader<Resource>::ArcResourceReader(){
    string dust;
    id_m = "-1";
    origin_m = -1;
    destination_m = -1;
    //        if (read.is) read >> dust;
    //        read >>
}

template<typename Resource>
ArcResourceReader<Resource>::ArcResourceReader(ifstream & read){
   throw;
//    string dust;
//    read >> dust >> id_m;
//    read >> dust >> origin_m;
//    read >> dust >> destination_m;
//        if (read.is) read >> dust;
//        read >>
}

template<typename Resource>
ArcResourceReader<Resource>::ArcResourceReader(stringstream & read){
    string dust;
      id_m = "A" + to_string(current_id);
      current_id++;
      read >> origin_m;
      read >> destination_m;
      resource_m = Resource(read);

    //        if (read.is) read >> dust;
    //        read >>
}

template<typename Resource>
InstanceOneResourceReader<Resource>::InstanceOneResourceReader(string file){
    arcs_m = VectorReaderAndWriter_ClassElements<ArcResourceReader<Resource> >::readFile('a',file);
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
               string origin, destination;
                l >> dust >> origin >> dust >> destination;
                origins_m = vector<string>(1,origin);
                destination_m = vector<string>(1,destination);
           }
       }
   }
if (!odFound) throw;
}

template<typename Resource>
void InstanceOneResourceReader<Resource>::computeArcResources(){
    // Add arcs
    arcResources_m.clear();
    for (typename vector<ArcResourceReader<Resource> >::iterator it = arcs_m.begin(); it != arcs_m.end(); ++it) {
        NewArcContextInformation<Resource> info;
        info.originVertexName = to_string(it->origin_m);
        info.destinationVertexName = to_string(it->destination_m);
        info.arcContextName = it->id_m;
        info.arcContextID = info.arcContextName;
        info.arcContextResource = it->resource_m;
        arcResources_m.insert(pair<string, NewArcContextInformation<Resource> >(info.arcContextID,info));

    }
}

template<typename Resource>
ContextGraph<Resource> * InstanceOneResourceReader<Resource>::getContextGraph(){
    computeArcResources();
    ContextBuilder<Resource> builder = ContextBuilder<Resource>();
    for (typename map<string, NewArcContextInformation<Resource> >::iterator it = arcResources_m.begin(); it != arcResources_m.end(); ++it) {
        builder.addContextArc(it->second);
    }

    for (vector<string>::iterator it = origins_m.begin(); it != origins_m.end(); ++it) {
        builder.addSourceVertex(*it);
    }

    for (vector<string>::iterator it = destination_m.begin(); it != destination_m.end(); ++it) {
        builder.addSinkVertex(*it);
    }

    return builder.getContextGraph();
}

} /* namespace latticeRCSP */
