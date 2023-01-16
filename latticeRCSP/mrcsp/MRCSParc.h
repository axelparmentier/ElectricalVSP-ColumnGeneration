/*
 * MRCSParc.h
 *
 *  Created on: 23 mars 2017
 *      Author: axelparmentier
 */

#ifndef SRC_MRCSP_MRCSPARC_H_
#define SRC_MRCSP_MRCSPARC_H_

#include "../latticeRCSP/Context/ArcContext.h"

using namespace latticeRCSP;

namespace mrcsp {

template <typename Resource>
struct MRCSParcInfo {
public:
   string id;
   string tail;
   string head;
   Resource resource;

   MRCSParcInfo(string id0, string tail0, string head0, Resource resource0){
      id = id0;
      tail = tail0;
      head = head0;
      resource = resource0;
   }
};

template <typename Resource>
class MRCSParc
{
   const string id_m;
   const string tail_m;
   const string head_m;
   Resource resource_m;
   ArcContext<Resource> * arcContext_m;

public:
   MRCSParc(MRCSParcInfo<Resource> info) : id_m(info.id), tail_m(info.tail), head_m(info.head)
   {
      resource_m = info.resource;
      arcContext_m = NULL;
   }

   const string getHeadM() const {return head_m;}
   const string getIdM() const {return id_m;}
   Resource getResourceM() const {return resource_m;}
   void setResourceM(Resource resourceM) {resource_m = resourceM;}
   const string getTailM() const {return tail_m;}
   ArcContext<Resource>* getArcContextM() const {return arcContext_m;}
   void setArcContextM(ArcContext<Resource> * ptr) {arcContext_m = ptr;}
};

} /* namespace mrcsp */

#endif /* SRC_MRCSP_MRCSPARC_H_ */
