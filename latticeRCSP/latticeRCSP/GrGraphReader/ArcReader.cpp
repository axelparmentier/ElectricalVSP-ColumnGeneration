//
//  ArcReader.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 22/09/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//

#include "ArcReader.h"
#include <sstream>



namespace latticeRCSP {
   int ArcReader::current_id = 0;

    ArcReader::ArcReader(){
        string dust;
        id_m = "-1";
        origin_m = -1;
        destination_m = -1;
        //        if (read.is) read >> dust;
        //        read >>
    }
    
    ArcReader::ArcReader(ifstream & read){
        string dust;
        read >> dust >> id_m;
        read >> dust >> origin_m;
        read >> dust >> destination_m;
//        if (read.is) read >> dust;
//        read >>
    }
    
    ArcReader::ArcReader(stringstream & read){
        string dust;
//        read >> dust >> id_m;
//        read >> dust >> origin_m;
//        read >> dust >> destination_m;
          id_m = current_id;
          current_id++;
          read >> origin_m;
          read >> destination_m;
        //        if (read.is) read >> dust;
        //        read >>
    }
}
