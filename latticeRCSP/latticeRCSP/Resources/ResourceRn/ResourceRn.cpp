//
//  resource.cpp
//  RCSP
//
//  Created by Axel Parmentier on 11/09/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ResourceRn.h"

#include <algorithm>

//void ResourceRn::whichType() const{
//    cout << "Type : resource" << endl;
//}
//
//int ResourceRn::nResources = -1;

namespace latticeRCSP {
    
//    int ResourceRn::keyIndex = 0;
    int ResourceRn::costIndex = 0;
    int ResourceRn::nResources = -1;
    //double ResourceRn::constraintCost = INFINITY;

ResourceRn::ResourceRn(){
//    nResources = constraintNResources;
    resourceValue = vector<double>(nResources,0);
//    costIndex = -1;
//    keyIndex = -1;
}

ResourceRn::ResourceRn(stringstream & read){
   string dust;
   read >> dust;
   int n;
   read >> n;
   if (nResources == -1){
      nResources = n;
   } else {
      if (n != nResources){
         throw "bad number of ressources in ResourceRn::ResourceRn(stringstream & read)" ;
      }
   }
   read >> dust;
   resourceValue = vector<double>(nResources, -1);
   for (int i = 0; i < nResources; ++i){
      read >> resourceValue[i];
   }
}

//resource * ResourceRn::Clone() const{
//    return new ResourceRn(*this);
//}

ResourceRn::ResourceRn(vector<double> & r){
    if (nResources != int(r.size())) throw "resource size is not good";
    resourceValue = r;
//    costIndex = 0;
//    keyIndex = 0;
}

//ResourceRn::ResourceRn(vector<double> & r, int costInd, int keyInd){
//#ifdef DEBUG_TESTS
//    if ((!(costInd < r.size() && keyInd < r.size())) || costInd < 0 || keyInd < 0) throw "Wronf indices";
//#endif
//    costIndex = costInd;
//    keyIndex = keyInd;
//    resourceValue = r;
//    nResources = int(r.size());
//}

//ResourceRn::ResourceRn(double c, vector<double> & v){
//    if (nResources == -1) {
//        nResources = v.size();
//    } else if (v.size() != nResources) throw "Resource must have the same size";
//    
//    resourceValue = v;
//    
//    isCostSet = true;
//    cost = c;
//}

//ResourceRn::ResourceRn(bool a, vector<double> & r){
//    if (a) {
//        if (nResources == -1) {
//            nResources = r.size() - 1;
//        } else if (r.size() - 1 != nResources) throw "Resource must have the same size";
//        
//        resourceValue = vector<double>(r.size()-1);
//        for (int i = 0; i<r.size()-1; i++) {
//            resourceValue[i] = r[i+1];
//        }
//        isCostSet = true;
//        cost = r[0];
//        
//    } else {
//        if (nResources == -1) {
//            nResources = r.size();
//        } else if (r.size() != nResources) throw "Resource must have the same size";
//        
//        resourceValue = r;
//        isCostSet = false;
//    }
//}

//bool ResourceRn::getIsCostSet() const{
//    return isCostSet;
//}

//----------------------------------------------
// Getters and Setters
//----------------------------------------------
//void ResourceRn::setCost(double c){
//#ifdef DEBUG_TESTS
//    if (costIndex < 0 || costIndex > nResources -1) throw "Cost is not set";
//#endif
//    resourceValue[costIndex] = c;
//}

void ResourceRn::setCostIndex(int i){
   costIndex = i;
}

int ResourceRn::getNResources() {
    return nResources;
}

double ResourceRn::getResourceValue(int r) const{
    return resourceValue[r];
}

const vector<double> & ResourceRn::getResourceValue() const{
    return resourceValue;
}

//int ResourceRn::getCostIndex() const{
//    return costIndex;
//}
//
//int ResourceRn::getKeyIndex() const{
//    return keyIndex;
//}


//void ResourceRn::setToInfinity(){
//    for (int i = 0; i<nResources; i++) {
//        resourceValue[i] = INFINITY;
//    }
//    if (isCostSet) {
//        cost = INFINITY;
//    }
//}

//---------------------------------------------------
// Mandatory methods to be used in the resource framework
//---------------------------------------------------

//---------------------
// Expand Resource
//---------------------

void ResourceRn::expand(const ResourceRn & r){
//#ifdef DEBUG_TESTS
//    if (nResources != r.getNResources()) throw "size problem";
//    if (costIndex != r.getCostIndex()) cout << "Not the same cost index in resource expansion" << endl;
//    if (keyIndex != r.getKeyIndex()) cout << "Not the same key Index in resource expansion" << endl;
//#endif
    for (int i = 0; i<nResources; i++) {
        resourceValue[i] += r.getResourceValue(i);
    }
    
}

//void ResourceRn::expandResourceRn(const resource & r){
//    if (isCostSet != r.getIsCostSet()) throw "Cost in only one for the two";
//    expandResourceWithoutCost(r);
//    if (isCostSet) cost += r.getCost();
//}

//void ResourceRn::expandAlongArc(const resource & r, int i){
//    cout << "expandAlongArc shoulc no be used for resource but only for Resource Label" << endl;
//    expandResourceRn(r);
//}

//void ResourceRn::expandAlongArcWithKey(const resource & r, int a, double k){
//    cout << "Expand with key shoul not be used for resource but only for resource with key" << endl;
//    expandResourceRn(r);
//}

//void ResourceRn::expandResourceWithoutCost(const resource & r){
//    for (int i = 0; i<nResources; i++) {
//        resourceValue[i] += r.getResourceValue(i);
//        if (resourceValue[i] == - INFINITY) resourceValue[i] = 0; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    }
//}
//
//resource ResourceRn::resourceExpansion(const resource & r) const{
//    resource sol = *this;
//    sol.expandResourceRn(r);
//    return sol;
//}
//
//resource ResourceRn::resourceExpansionWithoutCost(const resource & r) const{
//    resource sol = *this;
//    sol.expandResourceWithoutCost(r);
//    return sol;
//}

bool ResourceRn::isLeq(const ResourceRn & r) const{
#ifdef DEBUG_TESTS
    if (nResources != r.getNResources()) throw "Not comparable";
#endif
    
    for (int i = 0; i<nResources; i++) {
       if (resourceValue[i] > r.getResourceValue(i) + EPS){
          return false;
       }
    }
    return true;
}

bool ResourceRn::isGeq(const ResourceRn & r) const{
   for (int i = 0; i<nResources; i++) {
          if (resourceValue[i] < r.getResourceValue(i) - EPS){
             return false;
          }
       }
       return true;
}

void ResourceRn::meet(const ResourceRn & r) {
#ifdef DEBUG_TESTS
    if (nResources != r.getNResources()) throw "Not comparable";
#endif
    for (int i = 0; i<nResources; i++) {
        resourceValue[i] = std::min(resourceValue[i],r.getResourceValue(i));
    }
}

//void ResourceRn::meetWithoutCost(const ResourceRn & r){
//    for (int i = 0; i<nResources; i++) {
//        resourceValue[i] = min(resourceValue[i],r.getResourceValue(i));
//    }
//}

void ResourceRn::join(const ResourceRn & r) {
#ifdef DEBUG_TESTS
    if (nResources != r.getNResources()) throw "Not comparable";
#endif
    for (int i = 0; i<nResources; i++) {
        resourceValue[i] = std::max(resourceValue[i],r.getResourceValue(i));
    }
    
}

double ResourceRn::getCost() const{
    //if (costIndex < 0 || costIndex > nResources -1) throw "Cost is not set";
    return resourceValue[costIndex];
}

double ResourceRn::getKey() const{
//    return resourceValue[keyIndex];
   double result = 0;
   for (vector<double>::const_iterator it = resourceValue.begin(); it!= resourceValue.end(); ++it){
      result += *it;
   }
   return result;
}

void ResourceRn::setToInfimum(){
    for (int i=0; i<nResources; i++) {
        resourceValue[i]= -INFINITY;
    }
}

void ResourceRn::setToNeutral(){
    for (int i=0; i<nResources; i++) {
        resourceValue[i]= 0;
    }
}

void ResourceRn::setToSupremum(){
    for (int i=0; i<nResources; i++) {
        resourceValue[i]= INFINITY;
    }
}

double ResourceRn::distance(const ResourceRn & r) const{
#ifdef DEBUT_TESTS
    if (nResources != r.getNResources()) {
        throw "Not comparable"
    }
#endif
    double result = 0;
    for (int i = 0; i<nResources; i++) {
        if (resourceValue[i] < r.getResourceValue(i)) {
            result += r.getResourceValue(i) - resourceValue[i];
        } else result += resourceValue[i] - r.getResourceValue(i);
    }
    return result;
}

//void ResourceRn::joinWithoutCost(const resource & r){
//    for (int i = 0; i<nResources; i++) {
//        resourceValue[i] = max(resourceValue[i],r.getResourceValue(i));
//    }
//}
//
//list<int> * ResourceRn::getPath()  {
//    throw "Should no be used with resource but only with resource label";
//}
//
//double ResourceRn::getKey() const{
//    throw "should not be used";
//    return 0;
//}
//
//void ResourceRn::hasBeenExpanded(){
//    throw "should not be used with resource, only with resource label with key";
//}
//
//bool ResourceRn::getHasBeenExpanded() const{
//    throw "should not be used with resource, only with resource label with key";
//
//}
//
//void ResourceRn::setKey(double){
//    throw "should not be used with resource but with resourceLabelWithKey only";
//}


//--------------------------------------------------
// Constraint
//--------------------------------------------------
vector<double> ResourceRn::constraintValue = vector<double>(0);

//int ResourceRn::constraintNResources = 0;

void ResourceRn::setResourceConstraint(vector<double> &r){
//    constraintNResources = int(r.size());
    constraintValue = r;
}

bool ResourceRn::isResourceConstraintSatisfied() const{
#ifdef DEBUG_TESTS
//    if (nResources != constraintNResources) throw "NOT COMPARABLE";
#endif
//    bool result = true;
    for (int i= 0; i<nResources; i++) {
       if(resourceValue[i] > constraintValue[i] + EPS) return false;
//        result &= resourceValue[i] <= constraintValue[i];
//        if (!result) continue;
    }
    return true;
}

//double ResourceRn::getConstraintCost(){
//    return constraintValue[costIndex];
//}
//
//    double ResourceRn::getConstraintKey(){
//        return constraintValue[keyIndex];
//    }
    
    
//    ResourceRn ::ResourceRn(int nResou){
//        nResources = nResou;
//        keyIndex =0;
//        costIndex = 0;
//        resourceValue = vector<double>(nResou);
//        for (int i = 0; i<nResources; i++) {
//            resourceValue[i] = rand()%100000 /(double) 1000;
//        }
//    }


}
