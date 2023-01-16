//
//  LinearCombinationFunction.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 15/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "LinearCombinationFunction.h"

namespace latticeRCSP {

    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::proportionOfFirstFunctor = .5;
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::firstFunctorNormalization = 1;
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::secondFunctorNormalization = 1;
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::firstFunctorCoefficient = .5;
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::secondFunctorCoefficient = .5;
    
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    void LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::setFirstFunctorNormalization(double norm){
        if (! (norm > 0)) {
            throw "normalization must be posiitve";
        }
        firstFunctorNormalization = norm;
        firstFunctorCoefficient = proportionOfFirstFunctor / firstFunctorNormalization;
    }
    
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    void LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::setSecondFunctorNormalization(double norm){
        if (! (norm > 0)) {
            throw "normalization must be posiitve";
        }
        secondFunctorNormalization = norm;
        secondFunctorCoefficient = (1 - proportionOfFirstFunctor)/secondFunctorNormalization;
    }
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    void LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::setProportionOfFirstFunctor(double prop){
        proportionOfFirstFunctor = prop;
        firstFunctorCoefficient = proportionOfFirstFunctor / firstFunctorNormalization;
        secondFunctorCoefficient = (1 - proportionOfFirstFunctor)/secondFunctorNormalization;
    }
    
    
    template <typename Resource, typename FirstFunctor, typename SecondFunctor>
    double LinearCombinationFunction< Resource, FirstFunctor, SecondFunctor >::getKey(const Resource & res){
//        double value  = firstFunctorCoefficient * FirstFunctor::getKey(res) + secondFunctorCoefficient * SecondFunctor::getKey(res);

        return firstFunctorCoefficient * FirstFunctor::getKey(res) + secondFunctorCoefficient * SecondFunctor::getKey(res);
    }
}