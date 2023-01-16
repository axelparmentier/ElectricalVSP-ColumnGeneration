//
//  ContextGraph.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ContextGraph__
#define __latticeRCSP_v2__ContextGraph__

#define DEBUG_TESTS

#include <stdio.h>
#include <vector>
#include <sstream>
#include "../Graph/Path.h"
//#include "ResourceGraph.h"
#include "ArcContext.h"
#include "../Graph/Graph.h"
#include "../GraphBuilder/GraphBuilder.h"

using namespace std;
using namespace latticeRCSP;

namespace latticeRCSP {
    template <typename Resource>
    class ContextGraph{
    public:
        ContextGraph<Resource>(vector<ArcContext<Resource> * >, Graph<Resource> *);
        ContextGraph(GraphBuilder<Resource> const &);
        ~ContextGraph();
        
        Graph<Resource> const * getGraph() const;
        Path<Resource> rebuildPathForwardFromString(list<string> const& arcPath) const;
        Path<Resource> rebuildPathBackwardFromString(list<string> const& arcPath) const;
        
    private:
        vector<ArcContext<Resource> * > arcResources;
        Graph<Resource > * graph_m;
    };
}

#include "ContextGraph.hpp"

#endif /* defined(__latticeRCSP_v2__ContextGraph__) */
