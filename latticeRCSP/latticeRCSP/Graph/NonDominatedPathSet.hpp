//
//  NonDominatedPathSet.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 02/06/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "NonDominatedPathSet.h"

namespace latticeRCSP {
    
    template <typename Resource>
    NonDominatedPathSet<Resource>::NonDominatedPathSet(){
        
    }
    
    template <typename Resource>
        bool NonDominatedPathSet<Resource>::insertIfNonDominated(Path<Resource> const & path){
       int changeInSize = 0;
       return insertIfNonDominated(path, changeInSize);
    }

    template <typename Resource>
    bool NonDominatedPathSet<Resource>::insertIfNonDominated(Path<Resource> const & path, int & changeInSize){
        
        Resource const & insertedRes = path.getResource();
        list<double>::iterator iter;

        typename list<Path<Resource> >::iterator it;
        if (content.size() > 0){
           for (it = content.begin(); it!= content.end(); ){
              if (it->getResource().isLeq(insertedRes)) {
                  return false;
              }

              if (insertedRes.isLeq(it->getResource())){
                 --changeInSize;
                  typename list<Path<Resource> >::iterator deleteIter = it;
                  ++it;
                  content.erase(deleteIter);
              } else {
                  ++it;
              }
          }
        }
        

        content.push_back(path);
        ++changeInSize;
        return true;
    }
    
}
