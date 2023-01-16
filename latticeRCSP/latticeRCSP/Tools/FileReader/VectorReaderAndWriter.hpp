//
//  VectorReaderAndWriter.cpp
//  simpleRestaurantRequestSimulator
//
//  Created by Axel Parmentier on 14/02/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "VectorReaderAndWriter.h"

namespace ToolsAxel {
    template <typename Element>
    vector<Element> VectorReaderAndWriter<Element>::readFile(string filename){
        return VectorReaderAndWriter<Element>::readFile('e',filename);
//        ifstream read(filename.c_str());
//        if (read.is_open()) {
//            string line;
//            
//            // Number of elements
//            bool nrFound = false;
//            int nElements = -1;
//            read.clear();
//            read.seekg(0,ios::beg); // back to the beginning of the file
//            while (getline(read, line) && !nrFound) {
//                stringstream l(line);
//                char lineType;
//                l >> lineType;
//                if (lineType == 'n') {
//                    string dust;
//                    l >>  nElements;
//                    nrFound = true;
//                }
//            }
//            if (!nrFound) throw "Number of elements is not set";
//            
//            // Read the elements
//            vector<Element> solution(nElements,-1);
//            int i = 0;
//            // Read the arcs
//            read.clear();
//            read.seekg(0,ios::beg); // back to the beginning of the file
//            while (getline(read, line)) {
//                stringstream l(line);
//                char lineType;
//                l >> lineType;
//                if (lineType == 'e'){
//                    l >> solution[i];
//                    i++;
//                }
//            }
//            
//            if (i != nElements) throw "wrong number of elements";
//            
//            return solution;
//            
//        } else {
//            throw "Unable to open " + filename;
//        }
    }
    
    template <typename Element>
    vector<Element> VectorReaderAndWriter<Element>::readFile(char elementLineChar ,string filename){
        ifstream read(filename.c_str());
        if (read.is_open()) {
            string line;
            
            int nElements = 0;
            bool nrFound = true;
            while (getline(read, line)) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == elementLineChar){
                    nElements++;
                }
            }
            //
            // Read the elements
            vector<Element> solution(nElements);
            int i = 0;
            // Read the arcs
            read.clear();
            read.seekg(0,ios::beg); // back to the beginning of the file
            while (getline(read, line)) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == elementLineChar){
                    l >> solution[i];
                    i++;
                }
            }
            
            if (nrFound && i != nElements) throw "wrong number of elements";
            else if (!nrFound) nElements = i;
            
            return solution;
            
        } else {
            throw "Unable to open " + filename;
        }
    }
   
    
    template <typename Element>
    void VectorReaderAndWriter<Element>::writeFile(string filename, const vector<Element> &vectorWritten){
        ofstream write(filename.c_str());
        if (write.is_open()) {
            VectorReaderAndWriter<Element>::writeFile(write, vectorWritten);
        } else {
            throw "Unable to open " + filename;
        }
        
    }
    
    template <typename Element>
    void VectorReaderAndWriter<Element>::writeFile(ofstream &write, const vector<Element> &vectorWritten){
        write << "c Vector file" << endl;
        write << "c" << endl ;
        write << "c Lines beginning by c are comments" << endl;
        write << "c Line beginning by n contains the number of elemnts" << endl;
        write << "c Lines beginning by e contains value" << endl;
        write << "c Lines beginning by l contain loaderClients" << endl;
        write << "c" << endl;
        write << "c File begin" << endl;
        write << "c" << endl;
        write << "n " << vectorWritten.size() << endl;
        write << "c" << endl;
        for (int i = 0; i<vectorWritten.size(); i++){
write <<"e " << vectorWritten[i] << endl;;
        }
        write << "c" << endl;
        write << "c File end" << endl;
    }
    
    
    
    //----------------------------------------------------------------
    
    template <typename Element>
    vector<Element> VectorReaderAndWriter_ClassElements<Element>::readFile(string filename){
        return VectorReaderAndWriter_ClassElements<Element>::readFile('e',filename);
    }
    
    template <typename Element>
    vector<Element> VectorReaderAndWriter_ClassElements<Element>::readFile(char elementLineChar ,string filename){
        ifstream read(filename.c_str());
        if (read.is_open()) {
            string line;
            
            int nElements = 0;
            bool nrFound = true;
            while (getline(read, line)) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == elementLineChar){
                    nElements++;
                }
            }
            //
            // Read the elements
            vector<Element> solution(nElements);
            int i = 0;
            // Read the arcs
            read.clear();
            read.seekg(0,ios::beg); // back to the beginning of the file
            while (getline(read, line)) {
                stringstream l(line);
                char lineType;
                l >> lineType;
                if (lineType == elementLineChar){
                    solution[i] = Element(l);                    i++;
                }
            }
            
            if (nrFound && i != nElements) throw "wrong number of elements";
            else if (!nrFound) nElements = i;
            
            return solution;
            
        } else {
            throw "Unable to open " + filename;
        }
    }
    
    template <typename Element>
    void VectorReaderAndWriter_ClassElements<Element>::writeFile(string filename, vector<Element> &vectorWritten){
        ofstream write(filename.c_str());
        if (write.is_open()) {
            VectorReaderAndWriter_ClassElements::writeFile(write, vectorWritten);
        } else {
            throw "Unable to open " + filename;
        }
        
    }
    
    template <typename Element>
    void VectorReaderAndWriter_ClassElements<Element>::writeFile(ofstream &write, vector<Element> &vectorWritten){
        write << "c Vector file" << endl;
        write << "c" << endl ;
        write << "c Lines beginning by c are comments" << endl;
        write << "c Line beginning by n contains the number of elemnts" << endl;
        write << "c Lines beginning by e contains value" << endl;
        write << "c Lines beginning by l contain loaderClients" << endl;
        write << "c" << endl;
        write << "c File begin" << endl;
        write << "c" << endl;
        write << "n " << vectorWritten.size();
        write << "c" << endl;
        for (int i = 0; i<vectorWritten.size(); i++){
            write <<"e ";
            vectorWritten[i].save(write);
        }
        write << "c" << endl;
        write << "c File end" << endl;
    }
}

