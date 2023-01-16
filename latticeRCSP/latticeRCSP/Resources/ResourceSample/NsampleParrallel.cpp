//
//  NsampleParrallel.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 04/11/2015.
//  Copyright © 2015 Axel Parmentier. All rights reserved.
//



#include "Nsample.h"

#ifdef NSAMPLE_TRANSFORM
#ifdef NSAMPLE_PARALLEL_TRANSFORM

#include <algorithm>
#include <functional>
#include <numeric>
//#include <experimental/execution_policy>
#include <experimental/numeric>

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
    
    void Nsample::initializeCopy(const ResourceSample::Nsample & b){
        nSamples = b.getNsamples();
        samplesValue = b.getSampleValue();
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
    
    void Nsample::testComparability(const ResourceSample::Nsample & b) const {
        if (nSamples != b.getNsamples()) {
            throw "Wrong number of samples";
        }
    }
    
    void Nsample::initializeToZeroOfSameSizeIfNotInitialized(const ResourceSample::Nsample & b){
        if (nSamples ==0){
            nSamples = b.getNsamples();
            samplesValue = vector<double>(nSamples,0);
        }
    }
    
    void Nsample::expand(const ResourceSample::Nsample & b){
        initializeToZeroOfSameSizeIfNotInitialized(b);
        if (b.getNsamples() == 0) return;
        testComparability(b);
        
//        std::experimental::parallel::transform(std::experimental::parallel::par,samplesValue.begin(), samplesValue.end(), b.getSampleBeginIterator(), samplesValue.begin(), plus<double>());
    }
    
    
    void Nsample::meet(const ResourceSample::Nsample & b){
        if (nSamples == 0) {
            initializeToZeroOfSameSizeIfNotInitialized(b);
        }
        
        testComparability(b);
        
        double const & (*min) (double const &, double const &) = std::min<double>;
        transform(samplesValue.begin(), samplesValue.end(), b.getSampleBeginIterator(), samplesValue.begin(), min);
    }
    
    void Nsample::join(const ResourceSample::Nsample & b){
        initializeToZeroOfSameSizeIfNotInitialized(b);
        testComparability(b);
        
        double const & (*max) (double const &, double const &) = std::max<double>;
        transform(samplesValue.begin(), samplesValue.end(), b.getSampleBeginIterator(), samplesValue.begin(), max);
    }
    
    template<typename T>
    struct distance_l1 : public binary_function<T,T,T>
    {
        T operator()(const T &x, const T &y) const{
            return x < y ? y - x : x - y;
        }
    };
    
    double Nsample::distance(const ResourceSample::Nsample & b) const{
        //        return distance(&b);
        testComparability(b);
        
        double init = 0;
        return inner_product(samplesValue.begin(),samplesValue.end(),b.getSampleBeginIterator(),init,plus<double>(),distance_l1<double>()) /(double) nSamples;
    }
    
    template<typename T>
    struct sum_absolute_value : public binary_function<T,T,T>
    {
        T operator()(const T & res, const T &x) const{
            return x < 0 ? res -x : res + x;
        }
    };
    
    double Nsample::distanceToNeutral() const {
        if (nSamples == 0) return 0;
        
        double init = 0;
        return accumulate(samplesValue.begin(),samplesValue.end(),init,sum_absolute_value<double>())/(double) nSamples;
        
    }
    
    template<typename T>
    struct is_non_negative : public binary_function<bool,T,bool>
    {
        bool operator()(const bool & res, const T &x) const{
            return x < - EPS ? false : res;
        }
    };
    
    bool Nsample::isNonNegative() const {
        if (nSamples == 0) return true;
        else {
            bool init = true;
            return accumulate(samplesValue.begin(),samplesValue.end(),init,is_non_negative<double>());
        }
    }
    
    template<typename T>
    struct is_non_positive : public binary_function<bool,T,bool>
    {
        bool operator()(const bool & res, const T &x) const{
            return x > EPS ? false : res;
        }
    };
    
    bool Nsample::isNonPositive() const {
        if (nSamples == 0) return true;
        else {
            bool init = true;
            return accumulate(samplesValue.begin(),samplesValue.end(),init,is_non_positive<double>());
        }
    }
    
    template<typename T>
    struct isLeqFunctor : public binary_function<T,T,bool>
    {
        T operator()(const T &x, const T &y) const{
            return x < y + EPS ? true : false;
        }
    };
    
    bool Nsample::isLeq(const ResourceSample::Nsample & b ) const{
        if (nSamples == 0){
            return b.isNonNegative();
        } else if (b.getNsamples() == 0){
            return isNonPositive();
        }
        testComparability(b);
        
        return inner_product(samplesValue.begin(),samplesValue.end(),b.getSampleBeginIterator(),true,logical_and<bool>(),isLeqFunctor<double>()) ;
    }
    
    template<typename T>
    struct isGeqFunctor : public binary_function<T,T,bool>
    {
        T operator()(const T &x, const T &y) const{
            return x > y - EPS ? true : false;
        }
    };
    
    bool Nsample::isGeq(const ResourceSample::Nsample & b) const {
        if (nSamples == 0){
            return b.isNonPositive();
        } else if (b.getNsamples() == 0){
            return isNonNegative();
        }
        testComparability(b);
        
        return inner_product(samplesValue.begin(),samplesValue.end(),b.getSampleBeginIterator(),true,logical_and<bool>(),isGeqFunctor<double>()) ;
        
    }
    
    void Nsample::setToNeutral(){
        double init = 0;
        fill(samplesValue.begin(),samplesValue.end(),init);
    }
    
    template<typename T>
    struct negative_part_functor : public unary_function<T,T>
    {
        T operator()(const T &x) const{
            T neutral = 0;
            return x < neutral ? x : neutral;
        }
    };
    
    void Nsample::setToNegativePart(){
        transform(samplesValue.begin(), samplesValue.end(), samplesValue.begin(), negative_part_functor<double>());
        
    }
    
    template<typename T>
    struct positive_part_functor : public unary_function<T,T>
    {
        T operator()(const T &x) const{
            T neutral = 0;
            return x > neutral ? x : neutral;
        }
    };
    
    void Nsample::setToPositivePart(){
        transform(samplesValue.begin(), samplesValue.end(), samplesValue.begin(), positive_part_functor<double>());
        
    }
    
    double Nsample::expectation() const{
        return accumulate(samplesValue.begin(), samplesValue.end(),0.0)/(double) nSamples;
    }
    
    
    struct probaGreaterThanTau_functor {
        const double tau;
        probaGreaterThanTau_functor(double _tau) : tau(_tau) {}
        
        double operator()(const double res, const double & x) const {
            return x > tau + EPS ? res + 1 : res;
        }
    };
    
    double Nsample::probaStrictlyGreaterThanTau(double tau) const{
        return accumulate(samplesValue.begin(), samplesValue.end(), 0.0, probaGreaterThanTau_functor(tau));
    }
    
    double Nsample::conditionalValueAtRisk(double beta) const {
        vector<double> temp(nSamples);
        copy(samplesValue.begin(),samplesValue.end(),temp.begin());
        sort(temp.begin(),temp.end());
        int nbCompleteElements = floor(beta *(double) nSamples);
        double init = 0.0;
        double result = accumulate(temp.end()-nbCompleteElements,temp.end(),init,plus<double>());
        result += temp[temp.size()-nbCompleteElements - 1] * ( (beta *(double) nSamples) - (double) floor(beta *(double) nSamples));
        return result /= (beta *(double) nSamples);
    }
    
    template<typename T>
    struct isLeqIntFunctor : public binary_function<T,T,int>
    {
        T operator()(const T &x, const T &y) const{
            return x < y + EPS ? 1 : 0;
        }
    };
    
    double Nsample::weightedDistance(const ResourceSample::Nsample & b) const{
        double weight = inner_product(samplesValue.begin(),samplesValue.end(),b.getSampleBeginIterator(),0.0,plus<double>(),isLeqIntFunctor<double>());
        return (weight * distance(b))/(double) nSamples;
        
    }
}

#endif
#endif

