//
//  ResultsPrinter.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 29/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ResultsPrinter__
#define __latticeRCSP_v2__ResultsPrinter__

#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// This class to avoid passing as argument of the algorithm the ofstream.

//Je peux peut-être faire une classe avec la liste des differents stream qui peuvent etre utiles
//- un tableau de stream ouverts
//- pour chaque type de stream l'index du stream ouvert

namespace ToolsAxel {
    class AlgorithmResults{
        static bool isAtTheBeginningOfALine;
        static string endLineSeparator;


    public:
        static void printSeparatorIfNotAtTheBeginningOfALine(ofstream &);
        static void printEndLineSeparatorAndEndLine(ofstream &);

        static string separator;
        static bool additional_tests;
    };
 
    /*
    class ResultsPrinter {
        static bool isOfstreamOpen;
        static string filename;
        
        
    public:
        static ofstream write;
        
        static bool isOpen();
        static void openStreamIfNotOpened();
        static void setOfstream(string);
    };
    
    */
    
    
}

#endif /* defined(__latticeRCSP_v2__ResultsPrinter__) */
