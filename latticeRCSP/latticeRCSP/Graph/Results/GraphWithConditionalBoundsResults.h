//
//  GraphWithConditionalBoundsResults.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 09/03/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__GraphWithConditionalBoundsResults__
#define __latticeRCSP_v2__GraphWithConditionalBoundsResults__

#include <stdio.h>
#include <vector>
#include "../../Tools/ResultsPrinter.h"
using namespace ToolsAxel;

namespace latticeRCSP {
    class GraphWithConditionalBoundsResults{
    public:
        int nActiveVertices;
        double maxDistanceFromConditionalLowerBoundToConditionalUpperBound;
        vector<double> vertexDistanceFromLowerBoundToUpperBound;
        
    public:
        GraphWithConditionalBoundsResults();
        void initialize();
    };
}

#endif /* defined(__latticeRCSP_v2__GraphWithConditionalBoundsResults__) */
