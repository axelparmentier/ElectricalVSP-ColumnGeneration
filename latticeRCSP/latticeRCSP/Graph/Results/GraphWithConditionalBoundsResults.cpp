//
//  GraphWithConditionalBoundsResults.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "GraphWithConditionalBoundsResults.h"

namespace latticeRCSP {
    GraphWithConditionalBoundsResults::GraphWithConditionalBoundsResults() {
        initialize();
    }
    
    void GraphWithConditionalBoundsResults::initialize(){
        nActiveVertices = -1;
        maxDistanceFromConditionalLowerBoundToConditionalUpperBound = -1;
        
    }
}