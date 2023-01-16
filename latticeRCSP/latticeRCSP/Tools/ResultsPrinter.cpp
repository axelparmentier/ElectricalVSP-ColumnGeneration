//
//  ResultsPrinter.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 29/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResultsPrinter.h"

namespace ToolsAxel {
    
    bool AlgorithmResults::isAtTheBeginningOfALine = true;
    string AlgorithmResults::endLineSeparator = " \\\\";
    
    bool AlgorithmResults::additional_tests = true;
    string AlgorithmResults::separator = " & ";
    
    
    void AlgorithmResults::printEndLineSeparatorAndEndLine(ofstream & write){
        write << endLineSeparator << endl;
        isAtTheBeginningOfALine = true;
    }
    
    void AlgorithmResults::printSeparatorIfNotAtTheBeginningOfALine(ofstream & write){
        if (isAtTheBeginningOfALine) {
            isAtTheBeginningOfALine = false;
        } else {
            write << separator;
        }
    }
/*
    bool ResultsPrinter::isOfstreamOpen = false;
    string ResultsPrinter::filename = "tmp_algo_logs.txt";
    ofstream ResultsPrinter::write = ofstream();
    
    bool ResultsPrinter::isOpen(){
        return isOfstreamOpen;
    }
    void ResultsPrinter::openStreamIfNotOpened(){
        if (!isOfstreamOpen) {
            write = ofstream(filename, ios_base::app);
            if (!write.is_open()) {
                cout << "error with ofstream initializiaiton" << endl;
                throw "error with ofstream initializiaiton";
            }
        }
    }
    
    void ResultsPrinter::setOfstream(string name){
        filename = name;
        openStreamIfNotOpened();
    }
 */
    
}