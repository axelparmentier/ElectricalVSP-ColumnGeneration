//
//  Cluster.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "Cluster.h"


namespace latticeClustering {
    template <typename Resource, typename ClusterCostFunctor>
    Cluster<Resource,ClusterCostFunctor>::Cluster(){
        cost = 0;
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    Cluster<Resource,ClusterCostFunctor>::Cluster(vector<Resource const *> const & clus){
        container = ClusterContainer<Resource>(clus);
        cost = ClusterCostFunctor::getCost(container);
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    unsigned int Cluster<Resource,ClusterCostFunctor>::size_allElements() const{
        return container.size_allElements();
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    unsigned int Cluster<Resource,ClusterCostFunctor>::size_nonFixedElements() const{
        return container.size_nonFixedElements();
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    double Cluster<Resource,ClusterCostFunctor>::insertElement(int e, const Resource * elRes){
        double oldCost = cost;
        container.insertElement(e,elRes);
        cost = ClusterCostFunctor::getCost(container);
        return cost - oldCost;
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    double Cluster<Resource,ClusterCostFunctor>::removeElement(int e){
        double oldCost = cost;
        container.removeElement(e);
        cost = ClusterCostFunctor::getCost(container);
        return cost - oldCost;
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    void Cluster<Resource,ClusterCostFunctor>::clear(){
        container.clear();
        cost = 0;
    }
    
//    template <typename Resource, typename ClusterCostFunctor>
//    double Cluster<Resource,ClusterCostFunctor>::swap(int elementRemoved,int elementAdded, const Resource * elAdded){
//        double oldCost = cost;
//        container.swap(elementRemoved,elementAdded,elAdded);
//        cost = ClusterCostFunctor::getCost(container);
//        return cost - oldCost;
//    }
//    
    template <typename Resource, typename ClusterCostFunctor>
    double Cluster<Resource,ClusterCostFunctor>::getCost() const{
        return cost;
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    Resource Cluster<Resource,ClusterCostFunctor>::getJoin() {
        return container.getJoin();
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    Resource Cluster<Resource,ClusterCostFunctor>::getMeet() {
        return container.getMeet();
    }
    
    template <typename Resource, typename ClusterCostFunctor>
    vector<int> Cluster<Resource,ClusterCostFunctor>::getNewElements() const  {
        return container.getNewElements();
    }
}