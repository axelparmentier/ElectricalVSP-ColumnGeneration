//
//  Nsample.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 05/05/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "Nsample.h"

#ifndef NSAMPLE_TRANSFORM
#ifndef NSAMPLE_PARALLEL_TRANSFORM
#include <math.h>
#include <set>

namespace ResourceSample {
    Nsample::Nsample(){
        nSamples = 0;
        samplesValue = vector<double>(0);
    }
    
    Nsample::Nsample(vector<double> & val){
        nSamples = val.size();
        samplesValue = val;
    }
    
    Nsample::Nsample(const Nsample & b){
        initializeCopy(b);
    }
    
    void Nsample::initializeCopy(const ResourceSample::Nsample * b){
        nSamples = b->getNsamples();
        samplesValue = b->getSampleValue();
    }
    
    void Nsample::initializeCopy(const ResourceSample::Nsample & b){
        initializeCopy(&b);
    }
    
    size_t Nsample::getNsamples() const {
        return nSamples;
    }
    
    vector<double> Nsample::getSampleValue() const{
        return samplesValue;
    }
    
    double Nsample::getSampleI(int i) const{
        return samplesValue[i];
    }
    
    vector<double>::const_iterator Nsample::getSampleBeginIterator() const{
        return samplesValue.begin();
    }
    
    void Nsample::testComparability(const ResourceSample::Nsample * b) const {
        if (nSamples != b->getNsamples()) {
            throw "Wrong number of samples";
        }
    }
    
    void Nsample::testComparability(const ResourceSample::Nsample & b) const {
        testComparability(&b);
    }
    
    void Nsample::initializeToZeroOfSameSizeIfNotInitialized(const ResourceSample::Nsample * b){
        if (nSamples ==0){
            nSamples = b->getNsamples();
            samplesValue = vector<double>(nSamples,0);
        }
    }
    
    void Nsample::initializeToZeroOfSameSizeIfNotInitialized(const ResourceSample::Nsample & b){
        initializeToZeroOfSameSizeIfNotInitialized(&b);
    }

    
//    void Nsample::addSample(double newVal){
//        nSamples++;
//        samplesValue.push_back(newVal);
//    }
//    
//    void Nsample::addSamples(vector<double> & newVal){
//        nSamples += newVal.size();
//        samplesValue.reserve(nSamples);
//        samplesValue.insert(samplesValue.end(), newVal.begin(), newVal.end());
//    }
//    
//    void Nsample::sampleNnewSamples(int N){
//        cerr << "This method should never be called" << endl;
//        throw "This method should never be called" ;
//    }
    
    void Nsample::expand(const ResourceSample::Nsample & b){
        initializeToZeroOfSameSizeIfNotInitialized(b);
        if (b.getNsamples() == 0) return;
        testComparability(b);
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            *it += *bVal;
            ++bVal;
        }
    }
    
    
//    void Nsample::meet(const ResourceSample::Nsample & b){
//        meet(&b);
//    }
    
    
    void Nsample::meet(const ResourceSample::Nsample & b){
        if (nSamples == 0) {
            initializeToZeroOfSameSizeIfNotInitialized(b);
        }
        
        testComparability(b);
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*bVal < *it) *it = *bVal;
            ++bVal;
        }
    }
    
    void Nsample::join(const ResourceSample::Nsample & b){
        initializeToZeroOfSameSizeIfNotInitialized(b);
        testComparability(b);
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*bVal > *it) *it = *bVal;
            ++bVal;
        }
    }
    
//    void Nsample::join(const ResourceSample::Nsample & b){
//        join(&b);
//    }

    
    double Nsample::distance(const ResourceSample::Nsample * b) const{
        testComparability(b);
        
        double result = 0;
        vector<double>::const_iterator bVal = b->getSampleBeginIterator();
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            result += fabs(*bVal - *it);
            ++bVal;
        }
        
        return result/(double) nSamples;
    }
    
    double Nsample::distance(const ResourceSample::Nsample & b) const{
        return distance(&b);
    }
    
    double Nsample::distanceToNeutral() const {
        if (nSamples == 0) return 0;
        
        double result = 0;
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            result += fabs(*it);
        }
        
        return result/(double) nSamples;
    }
    
    bool Nsample::isNonNegative() const {
        if (nSamples == 0) return true;
        else {
            for (unsigned int i = 0; i < nSamples; i++) {
                if (samplesValue[i] < -EPS) {
                    return false;
                }
            }
            return true;
        }
    }
    
    bool Nsample::isNonPositive() const {
        if (nSamples == 0) return true;
        else {
            for (unsigned int i = 0; i < nSamples; i++) {
                if (samplesValue[i] > EPS) {
                    return false;
                }
            }
            return true;
        }
    }
    
    bool Nsample::isLeq(const ResourceSample::Nsample & b ) const{
        if (nSamples == 0){
            return b.isNonNegative();
        } else if (b.getNsamples() == 0){
            return isNonPositive();
        }
        testComparability(b);
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it > *bVal + EPS) return false;
            ++bVal;
        }
        
        return true;
    }
    
//    bool Nsample::isLeq(const ResourceSample::Nsample & b ) const{
//        return isLeq(&b);
//    }
    
    bool Nsample::isGeq(const ResourceSample::Nsample & b) const {
        if (nSamples == 0){
            return b.isNonPositive();
        }else if (b.getNsamples() == 0){
            return isNonNegative();
        }
        testComparability(b);
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it < *bVal - EPS) return false;
            ++bVal;
        }
        
        return true;
    }
    
//    bool Nsample::isGeq(const ResourceSample::Nsample & b) const {
//        return isGeq(&b);
//    }

    
    void Nsample::setToNeutral(){
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            *it = 0;
        }
    }
    
    void Nsample::setToNegativePart(){
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it > 0) *it = 0;
        }
    }
    
    void Nsample::setToPositivePart(){
        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it < 0) *it = 0;
        }
    }
    
//    void Nsample::setToSupremum(){
//        for (vector<double>::iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
//            *it = INFINITY;
//        }
//    }
    
    double Nsample::expectation() const{
        double result = 0;
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            result += *it;
        }
        result = result /(double) nSamples;
        return result;
    }
    
    double Nsample::probaStrictlyGreaterThanTau(double tau) const{
        double result = 0;
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it > tau + EPS) {
                result += 1;
            }
        }
        result = result /(double) nSamples;
        return result;
    }
    
    double Nsample::conditionalValueAtRisk(double beta) const {
        double dbleSize = beta *(double) nSamples;
        int maxSize = ceil(dbleSize);
        multiset<double> greatestValues;
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (greatestValues.size() < maxSize) {
                greatestValues.insert(*it);
            } else {
                set<double>::iterator begIt = greatestValues.begin();
                if (*begIt < *it) {
                    greatestValues.erase(begIt);
                    greatestValues.insert(*it);
                }
            }
        }
        
        set<double>::iterator greatValIt = greatestValues.begin();
        double result = *greatValIt *(double) ( dbleSize - maxSize + 1);
        
        for (++greatValIt; greatValIt != greatestValues.end(); ++greatValIt) {
            result += *greatValIt;
        }
        
        result /= dbleSize;
        
        return result;
    }
    
    double Nsample::weightedDistance(const ResourceSample::Nsample & b) const{
        double weight = 0;
        
        vector<double>::const_iterator bVal = b.getSampleBeginIterator();
        for (vector<double>::const_iterator it = samplesValue.begin(); it != samplesValue.end(); ++it) {
            if (*it < *bVal + EPS) {
                weight += 1;
            }
            ++bVal;
        }
        
        return (weight * distance(b))/(double) nSamples;
        
    }
}

#endif
#endif