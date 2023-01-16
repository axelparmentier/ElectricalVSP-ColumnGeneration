/*
 * InstanceOneResourceReader.h
 *
 *  Created on: 25 juil. 2016
 *      Author: axelparmentier
 */

#ifndef LATTICERCSP_V2_READERANDBUILDER_INSTANCEONERESOURCEREADER_H_
#define LATTICERCSP_V2_READERANDBUILDER_INSTANCEONERESOURCEREADER_H_

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <set>
#include "../Context/ContextGraph.h"
#include "../ReaderAndBuilder/ContextBuilder.h"


using namespace std;
namespace latticeRCSP {

template <typename Resource>
class ArcResourceReader {
public:

   string id_m;
    int origin_m;
    int destination_m;
    Resource resource_m;
    static int current_id;

    ArcResourceReader();
    ArcResourceReader(ifstream &);
    ArcResourceReader(stringstream &);

};

template <typename Resource>
class InstanceOneResourceReader {



    vector<ArcResourceReader<Resource> > arcs_m;
    map<string, NewArcContextInformation<Resource> > arcResources_m;
    vector<string> origins_m;
    vector<string> destination_m;

public:
    InstanceOneResourceReader(string file);
    ContextGraph<Resource> * getContextGraph();

protected:
    void computeArcResources();

};

} /* namespace latticeRCSP */

#include "InstanceOneResourceReader.hpp"

#endif /* LATTICERCSP_V2_READERANDBUILDER_INSTANCEONERESOURCEREADER_H_ */
