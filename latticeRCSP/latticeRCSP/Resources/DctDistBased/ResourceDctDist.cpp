//
//  ResourceDctDist.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ResourceDctDist.h"

namespace latticeRCSP {
    double ResourceDctDist::supremumDistance = INFINITY;
    
    ResourceDctDist::ResourceDctDist(){
        isSupremum = false;
        dist = DctDist(vector<double>(1,1));
    }
    
    ResourceDctDist::ResourceDctDist(stringstream & read){
        string dust;
        int l;
        int n;
        read >> dust >> l;
        read >> dust >> dust >> n;
        vector<double> f(l+n , 0);
        read >> dust;
        for (int i=0; i<n; i++) {
            read >> f[i + l];
        }
        dist = DctDist(f);
        dist.integrate();
        isSupremum = false;

    }
    
    void ResourceDctDist::expand(const latticeRCSP::ResourceDctDist & r){
        if (!isSupremum) {
            if (r.getIsSupremum()) {
                isSupremum = true;
            } else{
                DctDist otherDist = r.getDctDist();
                otherDist.derive();
                dist.derive();
                dist.convolution(otherDist);
                dist.integrate();
            }
        }
    }
    
    void ResourceDctDist::meet(const latticeRCSP::ResourceDctDist & r){
        if (!r.getIsSupremum()) {
            if (!isSupremum) {
                dist.meetSTcumulative(r.getDctDist());
            } else{
                isSupremum = r.getIsSupremum();
                dist = r.getDctDist();
            }
        }
    }
    
    void ResourceDctDist::join(const latticeRCSP::ResourceDctDist & r){
        if (!isSupremum) {
            if (r.getIsSupremum()) {
                isSupremum = true;
            } else dist.joinSTcumulative(r.getDctDist());
        }
    }
    
    double ResourceDctDist::distance(const latticeRCSP::ResourceDctDist & r) const{
        if (isSupremum != r.getIsSupremum()) {
            return supremumDistance;
        } else return dist.firstWassersteinDistance(r.getDctDist());
    }
    
    bool ResourceDctDist::isLeq(const latticeRCSP::ResourceDctDist & r) const{
        if (r.getIsSupremum()) {
            return true;
        } else if (isSupremum) return false;
        else return dist.leqSTcumulative(r.getDctDist());
    }
    
    bool ResourceDctDist::isGeq(const latticeRCSP::ResourceDctDist & r) const{
        if (isSupremum) {
            return true;
        } else if (r.getIsSupremum()) return false;
        else return dist.geqSTcumulative(r.getDctDist());
    }
    
    void ResourceDctDist::setToNeutral(){
        isSupremum = false;
        dist = DctDist(vector<double>(1,1));
    }
    
    void ResourceDctDist::setToSupremum() {
        isSupremum = true;
    }
    
    DctDist const & ResourceDctDist::getDctDist() const{
        return dist;
    }
    
    bool ResourceDctDist::getIsSupremum() const{
        return isSupremum;
    }
}