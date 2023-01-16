//
//  ArcReader.hpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 22/09/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#ifndef ArcReader_hpp
#define ArcReader_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace latticeRCSP {
    class ArcReader {
        string id_m;
        int origin_m;
        int destination_m;
        static int current_id;
    public:
        ArcReader();
        ArcReader(ifstream &);
        ArcReader(stringstream &);
       
        int getOrigin() const {return origin_m;};
        int getDestination() const {return destination_m;};
        string getID() const {return id_m;};
    };
}

#endif /* ArcReader_hpp */
