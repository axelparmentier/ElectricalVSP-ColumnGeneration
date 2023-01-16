/*
 * StateGraphBuildingResults.h
 *
 *  Created on: 23 août 2016
 *      Author: axelparmentier
 */

#ifndef LATTICERCSP_V2_GRAPH_RESULTS_STATEGRAPHBUILDINGRESULTS_H_
#define LATTICERCSP_V2_GRAPH_RESULTS_STATEGRAPHBUILDINGRESULTS_H_

namespace latticeRCSP {

class StateGraphBuildingResults
{
public:
   int nStatesFirstPhase;
   int nStatesSecondPhase;

   int nVertices;
   int nArcs;

   double timeFirstPhase;
   double timeSecondPhase;
   double timeThirdPhase;
   double totalTime;


   StateGraphBuildingResults();
   virtual ~StateGraphBuildingResults();
};

} /* namespace latticeRCSP */

#endif /* LATTICERCSP_V2_GRAPH_RESULTS_STATEGRAPHBUILDINGRESULTS_H_ */
