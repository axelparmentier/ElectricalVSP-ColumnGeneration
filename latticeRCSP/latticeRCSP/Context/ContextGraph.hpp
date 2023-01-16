//
//  ContextGraph.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/12/2014.
//  Copyright (c) 2014 Axel Parmentier. All rights reserved.
//

#include "ContextGraph.h"

namespace latticeRCSP {
    template<typename Resource>
    ContextGraph<Resource>::ContextGraph(vector<ArcContext<Resource> *> arcs0, Graph<Resource> * graph0){
        arcResources = arcs0;
        graph_m = graph0;
    }
    
    template<typename Resource>
    ContextGraph<Resource>::ContextGraph(GraphBuilder<Resource> const & gbuilder){
        list<pair<int, int> > arcList = gbuilder.getArcs();
        list<Resource> arcResource = gbuilder.getArcResources();
        
        //---------------------
        // Build the resource
        //---------------------
        
        arcResources = vector<ArcContext<Resource> * >(arcList.size());
        typename list<Resource>::const_iterator itRes = arcResource.begin();
        int i =0;
        for (list<pair<int, int> >::const_iterator it = arcList.begin(); it != arcList.end(); ++it) {
            stringstream ss;
            ss << "A" << i;
            arcResources[i] = new ArcContext<Resource>(ss.str(),*itRes);
//            arcResources[i] = new ArcContext<Resource>("A" + to_string(i),*itRes);
            i++;
            ++itRes;
        }
        
        //--------------------
        // Build the graph
        //--------------------
        graph_m = new Graph<Resource>(arcList,& arcResources, gbuilder.getIsOriginDestinationSet(), gbuilder.getOrigin(), gbuilder.getDestination());
        
    }
    
    template <typename Resource>
    ContextGraph<Resource>::~ContextGraph<Resource>(){
        for (int i = 0; i< arcResources.size(); i++){
            delete arcResources[i];
        }
        delete graph_m;
    }
    
    template <typename Resource>
    Graph<Resource> const * ContextGraph<Resource>::getGraph() const{
        return graph_m;
    }
    
    template<typename Resource>
	Path<Resource> ContextGraph<Resource>::rebuildPathForwardFromString(list<string> const& arcPath) const{

		// Parse the string
		list<string> arcIDlist = arcPath;

//        while (pathID.find('A') != string::npos){
//            size_t pos = pathID.find("A", 1);
//            arcIDlist.push_back(pathID.substr(0,pos));
//            pathID.erase(0,pos);
//        }

		int origin = graph_m->getArc(*arcIDlist.begin() + "_").getOrigin();
		Path<Resource> solution = Path<Resource>(false, origin);

		// Rebuild the string
		for (list<string>::iterator it = arcIDlist.begin(); it != arcIDlist.end(); ++it) {
			auto arc = graph_m->getArc(*it + "_");
			solution.expand(arc);
		}

		return solution;
	}

	template<typename Resource>
	Path<Resource> ContextGraph<Resource>::rebuildPathBackwardFromString(list<string> const& arcPath) const{
		assert(arcPath.size() > 0);

		Path<Resource> solution;

		for (auto it = arcPath.rbegin(); it != arcPath.rend(); ++it) {
			string arcId_l(*it + "_");
			if (it == arcPath.rbegin())
			{
				int origin = graph_m->getArc(arcId_l).getDestination();
				solution = Path<Resource>(true, origin);
			}
			Path<Resource> tmpPath = Path<Resource>(false, graph_m->getArc(arcId_l).getOrigin());
			tmpPath.expand(graph_m->getArc(arcId_l));
			tmpPath.expand(solution);
			solution = tmpPath;
		}

		return solution;
	}
    
}
