//
//  ResourceNSample.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 12/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include <math.h>
#include "ResourceNSample.h"
#include <iostream>
#include <fstream>
#include <sstream>



namespace ResourceSample {
    
    double ResourceNSample::distanceToInfimum = 1e5;
    double ResourceNSample::distanceToSupremum = 1e5;
    
    ResourceNSample::ResourceNSample(){
        type_m = NEUTRAL;
        sample_m = Nsample();
    }
    
    ResourceNSample::ResourceNSample(Nsample const & sa){
        type_m = SAMPLE;
        sample_m = sa;
    }
    
    ResourceNSample::ResourceNSample(ifstream & read){
        type_m = SAMPLE;
        int nSamples;
        string dust;
        read >> dust >> dust >> dust >> nSamples >> dust;
        vector<double> values(nSamples);
        for (vector<double>::iterator it = values.begin(); it != values.end(); ++it) {
            read >> *it;
        }
        sample_m = Nsample(values);
        
    }
    
    ResourceNSample::ResourceNSample(stringstream & read){
        type_m = SAMPLE;
        int nSamples;
        string dust;
        read >> dust >> dust >> dust >> nSamples >> dust;
        vector<double> values(nSamples);
        for (vector<double>::iterator it = values.begin(); it != values.end(); ++it) {
            read >> *it;
        }
        sample_m = Nsample(values);
        
    }
    
    ResourceNSample::SampleType ResourceNSample::getType() const {
        return type_m;
    }
    
    Nsample const & ResourceNSample::getNSample() const{
        return sample_m;
    }
    
    void ResourceNSample::expand(const ResourceSample::ResourceNSample & r){
        if (r.getType() == NEUTRAL) return;
        if (type_m ==NEUTRAL) {
            type_m = r.getType();
            sample_m = r.getNSample();
            return;
        }
        if ((type_m == INFIMUM && r.getType() == SUPREMUM) ||(type_m == SUPREMUM && r.getType() == INFIMUM)) {
            throw "Non defined";
        }
        if (type_m == INFIMUM) return;
        if (type_m == SUPREMUM) return;
        if (r.getType() == INFIMUM) {
            type_m = INFIMUM;
            sample_m.setToNeutral();
            return;
        }
        if (r.getType() == SUPREMUM){
            type_m = SUPREMUM;
            sample_m.setToNeutral();
            return;
        }
        sample_m.expand(r.getNSample());
    }
    
    
    void ResourceNSample::meet(const ResourceSample::ResourceNSample & r){
        if (type_m == INFIMUM) return;
        if (type_m == SUPREMUM){
            type_m = r.getType();
            sample_m = r.getNSample();
            return;
        }
        if (r.getType() == INFIMUM) {
            type_m = INFIMUM;
            sample_m.setToNeutral();
            return;
        }
        if (r.getType() == SUPREMUM) return;
        
        if (r.getType() == NEUTRAL) {
            if (type_m == NEUTRAL) return;
            sample_m.setToNegativePart();
            return;
        }
        if (type_m == NEUTRAL) {
            type_m = r.getType();
            sample_m = r.getNSample();
            sample_m.setToNegativePart();
            return;
        }
        
        sample_m.meet(r.getNSample());

    }
    
    void ResourceNSample::join(const ResourceSample::ResourceNSample & r){
        if (type_m == SUPREMUM) return;
        if (type_m == INFIMUM) {
            type_m = r.getType();
            sample_m = r.getNSample();
            return;
        }
        if (r.getType() == INFIMUM) return;
        if (r.getType() == SUPREMUM) {
            type_m = SUPREMUM;
            sample_m.setToNeutral();
            return;
        }
        
        if (r.getType() == NEUTRAL) {
            if (type_m == NEUTRAL) return;
            sample_m.setToPositivePart();
            return;
        }
        if (type_m == NEUTRAL) {
            type_m = r.getType();
            sample_m = r.getNSample();
            sample_m.setToPositivePart();
            return;
        }
        sample_m.join(r.getNSample());
        
    }
    
    double ResourceNSample::distance(const ResourceSample::ResourceNSample & r) const{
        if (type_m == SUPREMUM){
            if (r.getType() == SUPREMUM) return 0;
            if (r.getType() == INFIMUM) return distanceToInfimum + distanceToSupremum;
            return distanceToSupremum;
        }
        if (type_m == INFIMUM){
            if (r.getType() == INFIMUM) return 0;
            if (r.getType() == SUPREMUM) return distanceToSupremum + distanceToInfimum;
            return distanceToInfimum;
        }
        if (r.getType() == SUPREMUM) return distanceToSupremum;
        if (r.getType() == INFIMUM) return distanceToInfimum;
        
        if (type_m == NEUTRAL) {
            if (r.getType() == NEUTRAL) return 0;
            else return r.getNSample().distanceToNeutral();
        }
        if (r.getType() == NEUTRAL) return sample_m.distanceToNeutral();
        
        return sample_m.distance(r.getNSample());
    }
    
    bool ResourceNSample::isLeq(const ResourceSample::ResourceNSample & r) const {
        if (type_m == INFIMUM) return true;
        if (r.getType() == SUPREMUM) return true;
        if (type_m == SUPREMUM) return false;
        if (r.getType() == INFIMUM) return false;
        if (type_m == NEUTRAL){
            if (r.getType() == NEUTRAL) return true;
            return r.getNSample().isNonNegative();
        }
        if (r.getType() == NEUTRAL) return sample_m.isNonPositive();
        return sample_m.isLeq(r.getNSample());
    }
    
    bool ResourceNSample::isGeq(const ResourceSample::ResourceNSample & r) const{
        if (type_m == SUPREMUM) return true;
        if (r.getType() == INFIMUM) return true;
        if (type_m == INFIMUM) return false;
        if (r.getType() == SUPREMUM) return false;
        if (type_m == NEUTRAL){
            if (r.getType() == NEUTRAL) return true;
            return r.getNSample().isNonPositive();
        }
        if (r.getType() == NEUTRAL) return sample_m.isNonNegative();
        return sample_m.isGeq(r.getNSample());
    }
    
    
    void ResourceNSample::setToNeutral(){
        type_m = NEUTRAL;
        sample_m.setToNeutral();
    }
    
    void ResourceNSample::setToInfimum(){
        type_m = INFIMUM;
        sample_m.setToNeutral();
    }
    
    void ResourceNSample::setToSupremum(){
        type_m = SUPREMUM;
        sample_m.setToNeutral();
    }
    
    double ResourceNSample::expectation() const {
        if (type_m == INFIMUM) return -INFINITY;
        if (type_m == SUPREMUM) return +INFINITY;
        if (type_m == NEUTRAL) return 0;
        return sample_m.expectation();
    }
    
    double ResourceNSample::probaStrictlyGreaterThanTau(double tau) const {
        if (type_m == INFIMUM) return 0;
        if (type_m == SUPREMUM) return 1;
        if (type_m == NEUTRAL) return (tau < 0);
        return sample_m.probaStrictlyGreaterThanTau(tau);
    }
    
    double ResourceNSample::conditionalValueAtRisk(double beta) const {
        if (type_m == INFIMUM) return -INFINITY;
        if (type_m == SUPREMUM) return +INFINITY;
        if (type_m == NEUTRAL) return 0;
        return sample_m.conditionalValueAtRisk(beta);
    }
    
    double ResourceNSample::weightedDistance(const ResourceNSample & r) const{
//        if (r.getType() == SAMPLE && type_m == SAMPLE) {
//            return distance(r);
//        } else {
//            return sample_m.weightedDistance(r.getNSample());
//        }
        if (r.getType() != SAMPLE || type_m != SAMPLE) {
            return distance(r);
        } else {
            return sample_m.weightedDistance(r.getNSample());
        }
    }
    
    double WeightedDistanceResourceNSampleFunctor::getDistance(const ResourceNSample & r1, const ResourceNSample & r2){
        return r1.weightedDistance(r2);
    }
    
}