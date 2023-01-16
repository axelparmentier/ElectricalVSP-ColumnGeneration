//
//  VectorReaderAndWriter.h
//  simpleRestaurantRequestSimulator
//
//  Created by Axel Parmentier on 14/02/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __simpleRestaurantRequestSimulator__VectorReaderAndWriter__
#define __simpleRestaurantRequestSimulator__VectorReaderAndWriter__

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace ToolsAxel {
    
    template <typename Element>
    class VectorReaderAndWriter{
    public:
        static vector<Element> readFile(string filename);
        static vector<Element> readFile(char elementLineChar, string filename);
        static void writeFile(string filename, const vector<Element> & vectorWritten);
        static void writeFile(ofstream & write, const vector<Element> & vectorWritten);
    };
    
    template <typename Element>
    class VectorReaderAndWriter_ClassElements{
    public:
        static vector<Element> readFile(string filename);
        static vector<Element> readFile(char elementLineChar, string filename);
        
        static void writeFile(string filename, vector<Element> & vectorWritten);
        static void writeFile(ofstream & write, vector<Element> & vectorWritten);
    };
}

#include "VectorReaderAndWriter.hpp"

#endif /* defined(__simpleRestaurantRequestSimulator__VectorReaderAndWriter__) */
