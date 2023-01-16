/*
 * LabelDominanceAlgorithm.h
 *
 *  Created on: 26 juil. 2016
 *      Author: axelparmentier
 */

#ifndef LATTICERCSP_V2_ALGORITHM_LABELDOMINANCEALGORITHM_H_
#define LATTICERCSP_V2_ALGORITHM_LABELDOMINANCEALGORITHM_H_

#include <vector>

#include "LabelSettingAlgorithm.h"
#include "../Graph/NonDominatedPathSet.h"

using namespace std;

namespace latticeRCSP {

template <typename Resource, typename BoundedGraph>
class LabelDominanceAlgorithm : public LabelSettingAlgorithm<Resource, BoundedGraph>{
protected:
   vector<NonDominatedPathSet<Resource> > vertexNonDominatedPathSet;

public:
   LabelDominanceAlgorithm(BoundedGraph *);
   virtual ~LabelDominanceAlgorithm();
   int nNonDominated;
   int nBoundsCut;
   int nDominanceCut;

   //---------------------
   // Redefined methods compared to label setting algorithm
   //---------------------
   void solve();
protected:
   void initialPrints();
   void initializeReverseCandidatePathsIfNotInitialized();
   void expandCurrentPath();
   void finalPrintsAndTests();

};


//    ------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    class LabelDominanceAlgorithm_withMaxTimeMonitoring : public LabelDominanceAlgorithm<Resource, BoundedGraph> {
    public:
       LabelDominanceAlgorithm_withMaxTimeMonitoring(BoundedGraph * graph,clock_t maxDuration, int numberOfIterationsBetweenTwoTests, double minImprovePercent, int maxNumberOfIterations = INFINITY);;
        void solve();
    };

    //    ------------------------------------------------------------------------------------------
    template <typename Resource, typename BoundedGraph>
    class LabelDominanceAlgorithm_withLabelMapMaxSize : public LabelDominanceAlgorithm<Resource, BoundedGraph> {
    public:
       LabelDominanceAlgorithm_withLabelMapMaxSize(BoundedGraph *, unsigned int maxSize);
        void solve();
    };

} /* namespace latticeRCSP */

#include "LabelDominanceAlgorithm.hpp"

#endif /* LATTICERCSP_V2_ALGORITHM_LABELDOMINANCEALGORITHM_H_ */
