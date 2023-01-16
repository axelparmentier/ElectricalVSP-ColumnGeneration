/*
 * MRCSPvertex.h
 *
 *  Created on: 23 mars 2017
 *      Author: axelparmentier
 */

#ifndef SRC_MRCSP_MRCSPVERTEX_H_
#define SRC_MRCSP_MRCSPVERTEX_H_

#include <set>

using namespace std;

namespace mrcsp {

template<typename Resource>
class MRCSPvertexInfo {
public:
   string id;
   Resource resource;
   MRCSPvertexInfo(string id0, Resource resource0){
      id = id0;
      resource = resource0;
   }
};

template<typename Resource>
class MRCSPvertex
{
private:
   const string id_m;
   Resource resource_m;
   set<string> arcsContainingVertex;

public:
   MRCSPvertex(MRCSPvertexInfo<Resource> info) : id_m(info.id)
   {
      resource_m = info.resource;
   }

   const string& getId() const
   {
      return id_m;
   }

   Resource getResource() const
   {
      return resource_m;
   }

   void setResource(Resource resource)
   {
      this->resource_m = resource;
   }

   void addContainingArc(string arc){
      arcsContainingVertex.insert(arc);
   }

   void deleteContainingArc(string arc){
      arcsContainingVertex.erase(arc);
   }

   set<string> getArcsContainingVertex() const
   {
      return arcsContainingVertex;
   }
};

} /* namespace latticeRCSP */

#endif /* SRC_MRCSP_MRCSPVERTEX_H_ */
