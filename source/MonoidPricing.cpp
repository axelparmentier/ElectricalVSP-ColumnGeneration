/*
 * MonoidPricing.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: axel
 */
#include "MonoidPricing.h"

#include <list>
#include <ctime>
#include <functional>
#include <algorithm>
#include <math.h>

#include "Parameters.h"
#include "Route.h"
#include "BranchAndPriceStructs.h"

#include "ResourceCharge.h"

#include "Timer.h"

#define BIG 100000000
//#define DEBUG_RES
#ifdef DEBUG_RES
#include "SolutionChecker.h"
#endif

//#define DESACTIVATE_CANDIDATE_PATHS


MonoidPricing::MonoidPricing(bool approximatePricer_l) : Pricing() {
	// TODO Auto-generated constructor stub
	isInitialized = false;
	approximatePricer = approximatePricer_l;
}

void MonoidPricing::init()
{
	// Console
	if (params.cg.spdLogLevel == Parameters::trace) level_m = level::trace;
	else if (params.cg.spdLogLevel == Parameters::debug)level_m = level::debug;
	else if (params.cg.spdLogLevel == Parameters::info)level_m = level::info;
	else if (params.cg.spdLogLevel == Parameters::warn)level_m = level::warn;
	else if (params.cg.spdLogLevel == Parameters::err)level_m = level::err;
	else if (params.cg.spdLogLevel == Parameters::critical)level_m = level::critical;
	else if (params.cg.spdLogLevel == Parameters::off)level_m = level::off;

	if (!params.cg.isLogToConsole) latticeRCSP::Console::setToFileLogger(params.cg.logFileName);

	latticeRCSP::Console::setLevel(level::off); // Do not turn on here: turn it on only after problem is built

	ResourceEvsp::setMaxCharge(data.max_charge);

	Timer timer;
	timer.start();
	int total_arcs = buildStationArcs();
	buildBatches();
	if (params.isGraphSparsified || approximatePricer) total_arcs = sparsifyBatchesStationArcs();
	cout << data.name << ",init," << total_arcs << "," << timer.stop() << endl;

	buildProblems();
	buildStationArcsOfBatchArc_mapping();
	isInitialized = true;
	priceAllRoutesSmallerThanGap = false;
	gap = numeric_limits<double>::infinity();
	//
}

void MonoidPricing::buildCompletionBounds(){

}

int MonoidPricing::getDepotBatchProblem(int depot_l, int batch){
	assert(depot_l < data.depots.size());
	assert(batch < params.batch.nbBatches);

	return depot_l * params.batch.nbBatches + batch;
}

void MonoidPricing::buildBatches(){
	assert(params.batch.nbBatches > 0);
	verticesOfBatch = vector<vector<int> >(params.batch.nbBatches);

	vector<int> depotVertices_l;
	depotVertices_l.reserve(data.depots.size());
	vector<int> taskVertices_l;
	taskVertices_l.reserve(data.vertices.size());

	for (auto && vert : data.vertices){
		if (vert.type == Vertex::Depot || vert.type == Vertex::Vehicle) depotVertices_l.push_back(vert.id);
		else if (vert.type == Vertex::Task) taskVertices_l.push_back(vert.id);
	}
	taskVertices_l.shrink_to_fit();

	// buildVerticesBatch
	int batchSizes = ceil(taskVertices_l.size() /(double) params.batch.nbClusterByPartitions);
	for (int par = 0; par < params.batch.nbPartitions; ++par)
	{
		std::random_shuffle(taskVertices_l.begin(),taskVertices_l.end());
		for (int cl = 0; cl < params.batch.nbClusterByPartitions; ++cl){
			int batch = par * params.batch.nbClusterByPartitions + cl;
			int beg = cl * batchSizes;
			int end_l = min((int)taskVertices_l.size(), beg+batchSizes);
			verticesOfBatch[batch].reserve(depotVertices_l.size() + end_l - beg);
			verticesOfBatch[batch].insert(verticesOfBatch[batch].end(), depotVertices_l.begin(), depotVertices_l.end());
			verticesOfBatch[batch].insert(verticesOfBatch[batch].end(), taskVertices_l.begin() + beg, taskVertices_l.begin() + end_l);			
		}
	}

	// buildBatchStationArcs
	auto adj_matrix_stationArcs = vector<vector<list<StationArc *> > >(data.vertices.size(), vector<list<StationArc *> >(data.vertices.size()));
	for (auto && arc : stationArcs){
		adj_matrix_stationArcs[arc.from][arc.to].push_back(& arc);
	}

	int approx_size = (2 * stationArcs.size() * batchSizes) / (2 * data.vertices.size());
	stationArcsOfBatch = vector<vector<StationArc *> >(verticesOfBatch.size());
	for (auto && arcIdVect : stationArcsOfBatch) arcIdVect.reserve(approx_size);
	for (int b = 0; b < verticesOfBatch.size(); ++b)
	{
		for (auto && u : verticesOfBatch[b]){
			for (auto && v : verticesOfBatch[b]){
				stationArcsOfBatch[b].insert(stationArcsOfBatch[b].end(), \
				 adj_matrix_stationArcs[u][v].begin(), adj_matrix_stationArcs[u][v].end());
			}
		}
	}
	for (auto && arcIdVect : stationArcsOfBatch) arcIdVect.shrink_to_fit();

	//
	currentBatchOfDepot = vector<int>(data.depots.size(),0);
}

int MonoidPricing::buildProblems(){
	problems = vector<ProblemWithInfo>(data.depots.size() * params.batch.nbBatches);

	int total_arcs = 0;
	for (auto && it : data.depots)
		for (int b= 0; b < params.batch.nbBatches; ++b)
			total_arcs += buildProblem(it.id,b);

	return total_arcs;
}

void MonoidPricing::buildStationArcsOfBatchArc_mapping(){
	stationArcsOfBatchArc_mapping = vector<vector<vector< StationArc *> > >( \
		params.batch.nbBatches, vector<vector< StationArc *> >(data.edges.size()));
	for (int b = 0; b < params.batch.nbBatches; ++b){
		for (auto && pStArc : stationArcsOfBatch[b]){
			for (int i = 0; i < (int)pStArc->vertices.size(); i++)
			{
				int from = pStArc->vertices[i];
				int to = (i + 1 < (int)pStArc->vertices.size() ? pStArc->vertices[i + 1] : pStArc->to);
				stationArcsOfBatchArc_mapping[b][data.adj_indexes[from][to]].push_back(pStArc);
			}
		}
	}
}

int MonoidPricing::getDestinationId() const{
	return (int)data.vertices.size();
}

int MonoidPricing::getEdgeIdWithCharge(int withoutCharge, int stationId) const {
	assert(stationId != 0);
	return withoutCharge + stationId * (int)data.edges.size();
}

#ifdef MULTI_CHARGE

int MonoidPricing::buildStationArcs()
{
	int total_arcs = 0;
	initStations();
	if (!params.isSequencesOfStationsAuthorized || params.isGraphSparsified || approximatePricer) 
		total_arcs = buildStationArcsSparsified();
	else 
		total_arcs = buildStationArcsExact();
	Console::setLevel(level_m);
	return total_arcs;
}

void MonoidPricing::initStations(){
	stations.clear();
	stations.reserve(data.vertices.size());
	for (auto && vertex : data.vertices){
		if (vertex.type == Vertex::Station)
		{
			stations.push_back(vertex.id);
		}
	}
	stations.shrink_to_fit();
}

int MonoidPricing::buildStationArcsSparsified(){

	ResourceCharge::setChargeFactor(data.recharge_factor);
	ResourceCharge::setMaxCharge(data.max_charge);

	int notAdded = 0;
	stationArcs.clear();
	vector<StationArc> stationArcs_l;
	stationArcs_l.reserve(10 * data.vertices.size());
	int nVerticesPairs = 0;
	StationArc stationArc;

	for (auto && arc : data.edges){
		// Shared between non-dominated station arcs
		Vertex const & from = data.vertices[arc.from];
		Vertex const & to = data.vertices[arc.to];
		// int toId = to.id;
		if (from.type == Vertex::Station || to.type == Vertex::Station) continue;
		if (from.type == Vertex::Depot && to.type == Vertex::Depot) continue;

		stationArc.from = arc.from;
		stationArc.to = arc.to;

		if (from.type == Vertex::Depot) stationArc.depotId = arc.from;
		else if (to.type == Vertex::Depot) stationArc.depotId = arc.to;
		else stationArc.depotId = -1;	

		// Arc Without station
		stationArc.id = (int)stationArcs_l.size();
		stationArc.cost = arc.cost;
		stationArc.resourceStandard = ResourceEvsp(arc.cost, arc.cost, data.max_charge - arc.cost, -arc.cost);
		stationArc.resourceArtificial = ResourceEvsp(0, arc.cost, data.max_charge - arc.cost, -arc.cost);
		stationArc.vertices = vector<int>(1,arc.from);
		stationArcs_l.push_back(stationArc);
		++nVerticesPairs;

		if (to.type == Vertex::Vehicle) continue;

		// Arcs with stations

		double begTime = from.due;
		if (from.type == Vertex::Depot || from.type == Vertex::Vehicle) begTime = from.ready;
		double endTime = to.ready;
		if (to.type == Vertex::Depot || to.type == Vertex::Vehicle) endTime = to.due;
		assert(endTime > begTime - EPS);
		ResourceCharge::setMaxDistance(endTime - begTime);

		list< pair<ResourceCharge, int> > nonDominatedStations;

		for (auto && sta : stations){ 
			auto & sourceStationArc = data.edges[data.adj_indexes[arc.from][sta]];
			ResourceCharge resCharge = ResourceCharge(ResourceCharge::Source, sourceStationArc.cost);
			auto & stationSinkArc = data.edges[data.adj_indexes[sta][arc.to]];
			resCharge.expand(ResourceCharge(ResourceCharge::Sink,stationSinkArc.cost));

			// Remove this arc if it is unfeasible or dominated
			if (!ResourceChargeFunctions::isConstraintSatisfied(resCharge)) continue;

			bool isDominated = false;
			for (auto && nonDom : nonDominatedStations){
				if (nonDom.first.isLeq(resCharge)){
					isDominated = true;
					break;
				}
			}
			if (isDominated) continue;

			// Remove arcs dominated by this arc
			if (!nonDominatedStations.empty()){
				for (auto it = nonDominatedStations.begin(); it!= nonDominatedStations.end(); ){
					if (resCharge.isLeq(it->first)) it = nonDominatedStations.erase(it);
					else ++it;
				}
			}

			nonDominatedStations.push_back(pair<ResourceCharge, int>(resCharge, sta));
		}
		for (auto && nonDom : nonDominatedStations){
			ResourceCharge & res = nonDom.first;
			auto sta = nonDom.second;

			stationArc.id = (int)stationArcs_l.size();
			stationArc.cost = res.getTotalDistance();
			stationArc.resourceStandard = res.getResourceEvsp();
			stationArc.resourceArtificial = res.getResourceEvspArtificial();
			stationArc.vertices = {arc.from, sta};
			stationArcs_l.push_back(stationArc);	
		}
	}	
	stationArcs.clear();// = vector<StationArc>(stationArcs_l.size());
	stationArcs.insert(stationArcs.begin(), stationArcs_l.begin(), stationArcs_l.end());
	out << endl << stationArcs.size() << " non dominated stations arcs built before sparsification";
	out << ", which makes " << stationArcs.size() /(double) nVerticesPairs << " on average " << endl;

	return (int)stationArcs.size();
}

int MonoidPricing::buildStationArcsExact(){
	// Builds all the non-dominated sequences of the form

	ResourceCharge::setChargeFactor(data.recharge_factor);
	ResourceCharge::setMaxCharge(data.max_charge);

	stationArcs.clear();
	vector<StationArc> stationArcs_l;
	stationArcs_l.reserve(10 * data.vertices.size());
	MRCSParcInfo<ResourceCharge> infoArc("default", "default", "default", ResourceCharge());
	MRCSPvertexInfo<ResourceCharge> infoVert("default", ResourceCharge());
	MRCSPproblem<ResourceCharge, ResourceChargeFunctions> pbStations;
	int nVerticesPairs = 0;

	// Station -- Station Arcs Shared By All Problems
	for (auto && vert : data.vertices){
		if (vert.type == Vertex::Station){
			infoVert.id = to_string(vert.id);
			infoVert.resource = ResourceCharge();
			pbStations.addVertex(infoVert);
		}
	}

	for (auto && arc : data.edges){
		Vertex const & from = data.vertices[arc.from];
		Vertex const & to = data.vertices[arc.to];
		if (from.type != Vertex::Station || to.type != Vertex::Station) continue;
		if (arc.cost > data.max_charge) continue;
		infoArc.id = to_string(arc.id);
		infoArc.tail = to_string(from.id);
		infoArc.head = to_string(to.id);
		infoArc.resource = ResourceCharge(ResourceCharge::Standard, arc.cost);
		pbStations.addArc(infoArc);
	}

	// Solve one Problem for each nonStationEdge
	StationArc stationArc;
	for (auto && arc : data.edges){
		// Build ChargeGraphs
		Vertex const & from = data.vertices[arc.from];
		Vertex const & to = data.vertices[arc.to];
		if (from.type == Vertex::Depot && to.type == Vertex::Depot) continue;
		if (from.type == Vertex::Station || to.type == Vertex::Station) continue;


		// Update non Station Arcs
		stationArc.from = arc.from;
		stationArc.to = arc.to;
		if (from.type == Vertex::Depot) stationArc.depotId = arc.from;
		else if (to.type == Vertex::Depot) stationArc.depotId = arc.to;
		else stationArc.depotId = -1;
		// Arc Without station
		stationArc.id = (int)stationArcs_l.size();
		stationArc.cost = arc.cost;
		stationArc.resourceStandard = ResourceEvsp(arc.cost, arc.cost, data.max_charge - arc.cost, -arc.cost);
		stationArc.resourceArtificial = ResourceEvsp(0, arc.cost, data.max_charge - arc.cost, -arc.cost);
		stationArc.vertices = vector<int>(1,arc.from);
		stationArcs_l.push_back(stationArc);
		++nVerticesPairs;


		if (to.type == Vertex::Vehicle) continue;


		// Arcs with stations

		infoVert.id = to_string(from.id);
		pbStations.addVertex(infoVert);
		pbStations.addOrigin(infoVert.id);
		infoVert.id = to_string(to.id);
		pbStations.addVertex(infoVert);
		pbStations.addDestination(infoVert.id);
		list<string> arcsToDelete;
		// This part can be clearly imporved, using adjacency matrix
		list<int> ssEdges;
		for (auto && sta : stations){
			ssEdges.push_back(data.adj_indexes[from.id][sta]);
			ssEdges.push_back(data.adj_indexes[sta][to.id]);
		}

		for (auto && ssArcId : ssEdges){
			auto ssArc = data.edges[ssArcId];
		// for (auto && ssArc : data.edges){
			Vertex const & ssFrom = data.vertices[ssArc.from];
			Vertex const & ssTo = data.vertices[ssArc.to];
			bool isSource = (ssFrom.id == from.id && ssTo.type == Vertex::Station);
			bool isSink = (ssFrom.type == Vertex::Station && ssTo.id == to.id);
			if (isSource || isSink){
				infoArc.id = to_string(ssArc.id);
				infoArc.tail = to_string(ssFrom.id);
				infoArc.head = to_string(ssTo.id);
				if (isSource) infoArc.resource = ResourceCharge(ResourceCharge::Source, ssArc.cost);
				else infoArc.resource = ResourceCharge(ResourceCharge::Sink, ssArc.cost);
				pbStations.addArc(infoArc);
				arcsToDelete.push_back(infoArc.id);
			}
		}
		pbStations.setLabelAlgorithm(MRCSPproblem<ResourceCharge, ResourceChargeFunctions>::LabelAlgorithm::LabelDominance_withLabelMapMaxSize);
		pbStations.setNSolutionsSearched(BIG);

		double begTime = from.due;
		if (from.type == Vertex::Depot || from.type == Vertex::Vehicle) begTime = from.ready;
		double endTime = to.ready;
		if (to.type == Vertex::Depot || to.type == Vertex::Vehicle) endTime = to.due;
		assert(endTime > begTime - EPS);
		ResourceCharge::setMaxDistance(endTime - begTime);
#ifndef DESACTIVATE_CANDIDATE_PATHS
//		throw "Works only with candidate paths desactivated. Otherwise must remove dominated solutions";
#endif
		pbStations.solve();
		auto solutions = pbStations.getSolutionsVertexList();
		assert(solutions.size() != BIG);

		// Update non Station Arcs

		for (auto && it : solutions){
			stationArc.id = (int)stationArcs_l.size();
			ResourceCharge & res = it.second.second;
			stationArc.cost = res.getTotalDistance();
			stationArc.resourceStandard = res.getResourceEvsp();
			stationArc.resourceArtificial = res.getResourceEvspArtificial();

			auto vertices = it.second.first;
			assert(vertices.size() > 1);
			stationArc.vertices = vector<int>(vertices.size()-1);
			int i = 0;
			for (auto && stringId : vertices){
				if (i < stationArc.vertices.size()){
					stationArc.vertices[i] = stoi(stringId);
					++i;
				}
			}
			stationArcs_l.push_back(stationArc);
		}

		// Update chargeGraph
		for (auto && dArc: arcsToDelete){
			pbStations.deleteArc(dArc);
		}
		pbStations.deleteOrigin(to_string(from.id));
		pbStations.deleteVertex(to_string(from.id));
		pbStations.deleteDestination(to_string(to.id));
		pbStations.deleteVertex(to_string(to.id));
	}
	stationArcs.clear();// = vector<StationArc>(stationArcs_l.size());
	stationArcs.insert(stationArcs.begin(), stationArcs_l.begin(), stationArcs_l.end());
	out << endl << stationArcs.size() << " non dominated stations arcs built";
	out << ", which makes " << stationArcs.size() /(double) nVerticesPairs << " on average " << endl;

	return (int)stationArcs.size();
}

template <typename Value>
void appendMapToVec(vector<Value> & v, multimap<double,Value> const & m){
	for (auto && val : m){
		v.push_back(val.second);
	}
}

template <typename Value>
string printMap(multimap<double, Value> const & m){
	string result;
	for (auto && val : m){
		result += " " + to_string(val.first);
	}
	return result;
}

int MonoidPricing::sparsifyBatchesStationArcs(){

	// list<StationArc> mandatoryArcs;
	int total_arcs = 0;

	for (int batch = 0 ; batch < params.batch.nbBatches; ++batch){

		out << "Before Sparsification, " << stationArcsOfBatch[batch].size() << " arcs in batch " << batch << ". ";

		vector<multimap<double, StationArc*> > noChargeArcsFromDepot(data.vertices.size());
		vector<multimap<double, StationArc*> > noChargeArcsToDepot(data.vertices.size());
		vector<multimap<double, StationArc*> > chargeArcsFromDepot(data.vertices.size());
		vector<multimap<double, StationArc*> > chargeArcsToDepot(data.vertices.size());
		vector<multimap<double, StationArc*> > outgoingNoChargeArcs(data.vertices.size());
		vector<multimap<double, StationArc*> > outgoingChargeArcs(data.vertices.size());

		vector<multimap<double, StationArc*> > noChargeArcsFromVehicle(data.vertices.size());
		vector<multimap<double, StationArc*> > chargeArcsFromVehicle(data.vertices.size());

		for (auto && pArc : stationArcsOfBatch[batch]){
			double totalTime = data.vertices[pArc->to].ready - (data.vertices[pArc->from].due);
			double distance = ResourceEvspFunctions::getCost(pArc->resourceStandard);
			double value = 0;
			int nbArcsKept = params.spar.noChargeNbArcsKept;
			multimap<double, StationArc *> * pArcMap = nullptr;

			double incompressibleLostTime = totalTime - data.edges[data.adj_indexes[pArc->from][pArc->to]].cost;
			assert(pArc->depotId != -1 || incompressibleLostTime > - EPS || data.vertices[pArc->from].type == Vertex::Vehicle);

			if (pArc->depotId != -1) 
			{
				if (pArc->vertices.size() > 1)
				{
					value = params.spar.chargeDistanceCoef * distance;
					nbArcsKept = params.spar.chargeNbArcsDepotKept;
					if (data.vertices[pArc->from].type == Vertex::Depot) pArcMap = & chargeArcsFromDepot[pArc->to];
					else pArcMap = & chargeArcsToDepot[pArc->from];
				}
				else
				{
					value =  params.spar.noChargeDistanceCoef * distance;
					nbArcsKept = params.spar.noChargeNbArcsDepotKept;
					if (data.vertices[pArc->from].type == Vertex::Depot) pArcMap = & noChargeArcsFromDepot[pArc->to];
					else pArcMap = & noChargeArcsToDepot[pArc->from];
				}
			} 
			else if (data.vertices[pArc->from].type == Vertex::Vehicle) 
			{
				if (pArc->vertices.size() > 1)					
				{
					value = params.spar.chargeDistanceCoef * distance;
					nbArcsKept = params.spar.chargeNbArcsDepotKept;
					pArcMap = & chargeArcsFromVehicle[pArc->to];
				}
				else
				{
					value =  params.spar.noChargeDistanceCoef * distance;
					nbArcsKept = params.spar.noChargeNbArcsDepotKept;
					pArcMap = & noChargeArcsFromVehicle[pArc->to];
				}
			}
			else 
			{
				if (pArc->vertices.size() > 1)
				{
				// Recompute incompressibleLostTime
					double arrivalTime = data.vertices[pArc->from].due;
					arrivalTime += data.edges[data.adj_indexes[pArc->vertices[0]][pArc->vertices[1]]].cost + data.max_charge * data.recharge_factor; // full first charge
					for (int i = 1; i < pArc->vertices.size() - 1; ++i)
					{
						arrivalTime += data.edges[data.adj_indexes[pArc->vertices[i]][pArc->vertices[i+1]]].cost * (1 + data.recharge_factor); //full next recharge
					}
					arrivalTime += data.edges[data.adj_indexes[pArc->vertices.size() - 1][pArc->to]].cost * (1 + data.recharge_factor); 
					incompressibleLostTime = data.vertices[pArc->to].ready - arrivalTime;
					if (incompressibleLostTime < 0) incompressibleLostTime = 0;
					
					value = params.spar.chargeTimeCoef * totalTime;
					value += params.spar.chargeDistanceCoef * distance;
					value += params.spar.chargeIncompressibleLostTimeCoef * incompressibleLostTime;

					nbArcsKept = params.spar.chargeNbArcsKept;
					pArcMap = & outgoingChargeArcs[pArc->from];
				}
				else
				{
					value = params.spar.noChargeTimeCoef * totalTime;
					value +=  params.spar.noChargeDistanceCoef * distance;
					value += params.spar.noChargeIncompressibleLostTimeCoef * incompressibleLostTime;

					nbArcsKept = params.spar.noChargeNbArcsKept;
					pArcMap = & outgoingNoChargeArcs[pArc->from];
				}
			}

			pArcMap->insert(pair<double, StationArc *>(value,pArc));
			if (pArcMap->size() > nbArcsKept) {
				auto arcIt = pArcMap->end();
				--arcIt;
				pArcMap->erase(arcIt);
			}
		}


		// update station Arcs
		stationArcsOfBatch[batch].clear();
		stationArcsOfBatch[batch].reserve(data.vertices.size() * \
			(2*(params.spar.chargeNbArcsDepotKept + params.spar.noChargeNbArcsDepotKept) + params.spar.noChargeNbArcsKept + params.spar.chargeNbArcsKept));

		// stationArcsOfBatch[batch].insert(stationArcsOfBatch[batch].end(), mandatoryArcs.begin(), mandatoryArcs.end());
		for (int v = 0; v < data.vertices.size(); ++v){
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], noChargeArcsFromDepot[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], noChargeArcsToDepot[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], chargeArcsFromDepot[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], chargeArcsToDepot[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], noChargeArcsFromVehicle[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], chargeArcsFromVehicle[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], outgoingNoChargeArcs[v]);
			appendMapToVec<StationArc * >(stationArcsOfBatch[batch], outgoingChargeArcs[v]);
		}
		stationArcsOfBatch[batch].shrink_to_fit();

		total_arcs += (int)stationArcsOfBatch[batch].size();
		out << "After sparsfication, it remains " << stationArcsOfBatch[batch].size() << endl;
	}
	return total_arcs;
}

#elif
// void MonoidPricing::buildStationArcs(){
// 	int notAdded = 0;
// 	int added = 0;
// 	int dominated = 0;
// 	int nonDominated = 0;

// 	for (auto && arc : data.edges){
// 		Vertex const & from = data.vertices[arc.from];
// 		Vertex const & to = data.vertices[arc.to];
// 		int toId = to.id;
// 		if (from.type == Vertex::Station || to.type == Vertex::Station) continue;
// 		if (from.type == Vertex::Depot && from.id != depotId) continue;
// 		if (to.type == Vertex::Depot && to.id != depotId) continue;
// 		if (from.type == Vertex::Depot && to.type == Vertex::Depot) continue;
// 		if (to.type == Vertex::Depot) {
// 			toId = getDestinationId();
// 		}
// 		MRCSParcInfo<ResourceEvsp> infoArc("default", "default", "default", ResourceEvsp());
// 		infoArc.tail = to_string(from.id);
// 		infoArc.head = to_string(toId);
// 		infoArc.id = to_string(arc.id);
// 		assert(arc.cost < data.max_charge + EPS);
// 		infoArc.resource = ResourceEvsp((artificials ? 0.0 : arc.cost), \
// 				arc.cost, \
// 				std::numeric_limits<double>::infinity(), \
// 				-arc.cost); // Nota Bene: I take arc.cost as consumption on arc
// //			infoArc.resource = ResourceEvsp((artificials ? 0.0 : arc.cost), 0); // Nota Bene: I take arc.cost as consumption on arc
// 		pb.addArc(infoArc);
// 		++added;

// 		map<double, pair<double, int> > nonDominatedStations;

// 		for (auto && sta : stations){
// 			// Coudld be improved by adding only "non-dominated arcs
// 			Edge const & fromToSta = data.edges[data.adj_matrix[from.id][sta]];
// 			Edge const & staToTo = data.edges[data.adj_matrix[sta][to.id]];
// 			if (from.due + fromToSta.cost + staToTo.cost > to.ready) continue;

// 			// Dominance by noStation
// 			if (!(fromToSta.cost < data.max_charge + EPS) || !(staToTo.cost < data.max_charge + EPS)){
// 				++notAdded;
// 				continue;
// 			}
// 			depotId;
// 			double ready = (to.id == depotId ? to.due : to.ready);
// 			double due = (from.id == depotId ? from.ready : from.due);
// 			double chargeSta = (ready - (fromToSta.cost + staToTo.cost + due)) / data.recharge_factor;
// 			if (chargeSta > data.max_charge) chargeSta = data.max_charge;
// 			if (fromToSta.cost - arc.cost > - EPS){
// 				if (arc.cost < fromToSta.cost + staToTo.cost - chargeSta){
// 					++notAdded;
// 					continue;
// 				}
// 			}

// 			// Dominance by other stations(remove if there is stations capacity)
// 			auto it = nonDominatedStations.upper_bound(fromToSta.cost);
// 			if (nonDominatedStations.empty() || it == nonDominatedStations.begin()) {
// 				nonDominatedStations.insert(pair<double, pair<double, int> >(fromToSta.cost,pair<double, int>(staToTo.cost, sta)));
// 				continue;
// 			}
// 			--it;
// 			if (staToTo.cost >= it->second.first) {
// 				++dominated;
// 				continue;
// 			}
// 			it = nonDominatedStations.insert(pair<double, pair<double, int> >(fromToSta.cost,pair<double, int>(staToTo.cost, sta))).first;
// 			++it;
// 			while (it != nonDominatedStations.end()) {
// 				auto itCopy = it;
// 				++it;
// 				if (itCopy->second.first >= staToTo.cost) {
// 					nonDominatedStations.erase(itCopy);
// 					++dominated;
// 				}
// 				else break;
// 			}
// 		}
// 		for (auto && nonDom : nonDominatedStations){
// 			auto sta = nonDom.second.second;

// 			// buildStationArc
// 			StationArc stationArc;
// 			stationArc.station = sta;
// 			stationArc.id = to_string(getEdgeIdWithCharge(arc.id, sta));
// 			Edge const & fromToSta = data.edges[data.adj_matrix[from.id][sta]];
// 			Edge const & staToTo = data.edges[data.adj_matrix[sta][to.id]];
// 			double ready = (to.id == depotId ? to.due : to.ready);
// 			double due = (from.id == depotId ? from.ready : from.due);
// 			stationArc.charge = (ready - (fromToSta.cost + staToTo.cost + due)) / data.recharge_factor;
// 			stationArc.fromToStaCost = fromToSta.cost;
// 			stationArc.staToToCost = staToTo.cost;
// 			stationArcs.push_back(stationArc);

// 			//addStationArcs
// 			infoArc.id = stationArc.id;
// 			infoArc.resource = ResourceEvsp(-duals->vertices[stationArc.station] + (artificials ? 0.0 : stationArc.fromToStaCost + stationArc.staToToCost), \
// 					max(stationArc.fromToStaCost, stationArc.fromToStaCost - stationArc.charge + stationArc.staToToCost), \
// 					data.max_charge - stationArc.charge + stationArc.fromToStaCost , \
// 					-stationArc.fromToStaCost + stationArc.charge - stationArc.staToToCost);
// 			pb.addArc(infoArc);

// 			++added;
// 			++nonDominated;
// 		}
// 	}
// }
#endif

int MonoidPricing::buildProblem(int depotId, int batch_l)
{
//	ResourceEvsp::setMaxCharge(data.max_charge);
//	problems[depotId] = EvspProblem();
	ProblemWithInfo & pbWithInfo = problems[getDepotBatchProblem(depotId, batch_l)];
	pbWithInfo.problem = EvspProblem();
	EvspProblem & pb = pbWithInfo.problem;
	pbWithInfo.depot = depotId;
	pbWithInfo.vertexBatch = batch_l;
	pb.setIsVertexResourceActivated(true);
	// Vertices
	MRCSPvertexInfo<ResourceEvsp> infoVert("default", ResourceEvsp());

//	list<int> stations;

	out << "Start building graph of depot " << depotId << endl;

	// arc_mapping.resize(data.edges.size());

	// for (auto && vert : data.vertices){
	for (auto && vertId : verticesOfBatch[batch_l])
	{
		Vertex const & vert = data.vertices[vertId];
		infoVert.id = to_string(vert.id);
		if (vert.type == Vertex::Depot && vert.id == depotId)
		{
			// Not created for other depots
			// Origin vertex
			if (data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge)
			{
				infoVert.resource = ResourceEvsp(-duals->vertices[vert.id]
					+ (artificials ? 0.0 : data.vehicleWithInitialPosition_cost),
					0,
					(double)data.max_charge,
					(double)data.max_charge);
			}
			else 
			{
				infoVert.resource = ResourceEvsp(-duals->vertices[vert.id] + (artificials ? 0.0 : data.vehicle_cost),
					0,
					(double)data.max_charge,
					(double)data.max_charge);
			}
			pb.addVertex(infoVert);
			pb.addOrigin(infoVert.id);
			// Destination vertex
			infoVert.id = to_string(getDestinationId());
			infoVert.resource = ResourceEvsp();
			pb.addVertex(infoVert);
			pb.addDestination(infoVert.id);
		}
		else if (vert.type == Vertex::Task)
		{
			infoVert.resource = ResourceEvsp(-duals->vertices[vert.id],
					vert.consumption,
					(double)data.max_charge - vert.consumption,
					-vert.consumption);
			pb.addVertex(infoVert);
		}
		else if (vert.type == Vertex::Vehicle && data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge)
		{
			infoVert.resource = ResourceEvsp(-duals->vertices[vert.id],
					0, vert.initialCharge, vert.initialCharge);
			pb.addVertex(infoVert);
		}
	}

	MRCSParcInfo<ResourceEvsp> infoArc("default", "default", "default", ResourceEvsp());
	
	// for (int a = 0; a < (int)stationArcs.size(); a++)
	for (auto && pStArc : stationArcsOfBatch[batch_l])
	{
		if (pStArc->depotId != -1 && pStArc->depotId != depotId) continue;
		if (!data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge && data.vertices[pStArc->from].type == Vertex::Vehicle) continue;
		infoArc.id = to_string(pStArc->id);
		infoArc.tail = to_string(pStArc->from);
		infoArc.head = to_string(pStArc->to);
		if (data.vertices[pStArc->to].type == Vertex::Depot) infoArc.head = to_string(getDestinationId());
		infoArc.resource = pStArc->resourceArtificial;
		pb.addArc(infoArc);
	}

	pbWithInfo.previousArtificials = artificials;
	pbWithInfo.previousOptimal = true;
	pbWithInfo.previousValue = - 10; 

	return pb.getNumArcs();
}

// void MonoidPricing::updateProblems(){
// 	for (auto && it : data.depots){
// 		updateProblem(it.id);
// 	}
// }

void MonoidPricing::updateProblem(int depotId, int batch_l){

	ProblemWithInfo & pbWithInfo = problems[getDepotBatchProblem(depotId, batch_l)];
	EvspProblem & pb = pbWithInfo.problem;


	for (auto && vertId : verticesOfBatch[batch_l]){
		Vertex const & vert = data.vertices[vertId];
		if (vert.type == Vertex::Depot && vert.id == depotId){
			if (data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge) {
				pb.updateVertexResource(to_string(vert.id), ResourceEvsp(-duals->vertices[vert.id] \
					+ (artificials ? 0.0 : data.vehicleWithInitialPosition_cost),\
					0, \
					(double)data.max_charge, \
					(double)data.max_charge));
			}
			else {
				pb.updateVertexResource(to_string(vert.id), ResourceEvsp(-duals->vertices[vert.id] + (artificials ? 0.0 : data.vehicle_cost),\
					0, \
					(double)data.max_charge, \
					(double)data.max_charge));
			}
		}
		else if (vert.type == Vertex::Task){
			pb.updateVertexResource(to_string(vert.id), \
					ResourceEvsp(-duals->vertices[vert.id], \
										vert.consumption, \
										(double)data.max_charge - vert.consumption, \
										-vert.consumption));
		}
		else if (vert.type == Vertex::Vehicle && data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge){
			pb.updateVertexResource(to_string(vert.id), \
				ResourceEvsp(-duals->vertices[vert.id], \
					0, vert.initialCharge, vert.initialCharge));
		}
	}

#ifdef MULTI_CHARGE
	// for (auto& stationArc : stationArcs)
	for (auto && pStArc : stationArcsOfBatch[batch_l])
	{

		if (pStArc->depotId != -1 && pStArc->depotId != depotId) continue;
		if (!data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge && data.vertices[pStArc->from].type == Vertex::Vehicle) continue;

		if (artificials) pStArc->resourceArtificial.setReducedCost(0.0);
		else pStArc->resourceStandard.setReducedCost(pStArc->cost);
	}

	if (bounds != nullptr)
	{
		for (int b = 0; b < bounds->info.size(); b++)
		{
			if (bounds->info[b].key.type != FullEdge) continue;
			int e = bounds->info[b].key.id;
			for (auto && pStArc : stationArcsOfBatchArc_mapping[batch_l][e])
			{
				if (artificials) pStArc->resourceArtificial.addToReducedCost(-duals->bounds[b]);
				else pStArc->resourceStandard.addToReducedCost(-duals->bounds[b]);
			}
		}
	}

	// for (auto& stationArc : stationArcs)
	for (auto && pStArc : stationArcsOfBatch[batch_l])
	{
		if (pStArc->depotId != -1 && pStArc->depotId != depotId) continue;
		if (!data.depots[depotId].isArtificalDepotForVehiclesWithInitialCharge && data.vertices[pStArc->from].type == Vertex::Vehicle) continue;

		if (artificials) pb.updateArcResource(to_string(pStArc->id), pStArc->resourceArtificial);
		else pb.updateArcResource(to_string(pStArc->id), pStArc->resourceStandard);
	}
#elif
// 	if (artificials != previousArtificials[depotId]){
// 		previousArtificials[depotId] = artificials;
// //		out << endl << "Update Arcs" << endl;
// 		for (auto && arc : data.edges){
// 			Vertex const & from = data.vertices[arc.from];
// 			Vertex const & to = data.vertices[arc.to];
// 			int toId = to.id;
// 			if (from.type == Vertex::Station || to.type == Vertex::Station) continue;
// 			if (from.type == Vertex::Depot && from.id != depotId) continue;
// 			if (to.type == Vertex::Depot && to.id != depotId) continue;
// 			if (from.type == Vertex::Depot && to.type == Vertex::Depot) continue;
// 			if (to.type == Vertex::Depot) {
// 				toId = getDestinationId();
// 			}
// 			pb.updateArcResource(to_string(arc.id), \
// 					ResourceEvsp((artificials ? 0.0 : arc.cost), \
// 									arc.cost, \
// 									std::numeric_limits<double>::infinity(), \
// 									-arc.cost) ); // Nota Bene: I take arc.cost as consumption on arc
// 	//		infoArc.resource = ResourceEvsp((artificials ? 0.0 : arc.cost), 0); // Nota Bene: I take arc.cost as consumption on arc
// 			}
// 		for (auto && stationArc : stationArcs){
// 			assert(fabs(duals->vertices[stationArc.station]) < EPS);
// 			if (fabs(duals->vertices[stationArc.station]) >= EPS){
// 				out << "fabs(duals->vertices[stationArc.station]) " << fabs(duals->vertices[stationArc.station]) << endl;
// 			}
// 			// WARNING : MUST BE UPDATED AT ALL ITERATIONS IF NON ZERO
// 			ResourceEvsp res = ResourceEvsp(-duals->vertices[stationArc.station] + (artificials ? 0.0 : stationArc.fromToStaCost + stationArc.staToToCost), \
// 					max(stationArc.fromToStaCost, stationArc.fromToStaCost - stationArc.charge + stationArc.staToToCost), \
// 					data.max_charge - stationArc.charge + stationArc.fromToStaCost , \
// 					-stationArc.fromToStaCost + stationArc.charge - stationArc.staToToCost);
// 			pb.updateArcResource(stationArc.id,res);
// 		}
// 	}
#endif
}

bool MonoidPricing::do_price() {
	if (!isInitialized) init();
	int lastBatch = currentBatchOfDepot[depot];
	while (!priceBatch(currentBatchOfDepot[depot])){
		++currentBatchOfDepot[depot];
		currentBatchOfDepot[depot] %= params.batch.nbBatches;
		if (currentBatchOfDepot[depot] == lastBatch){
			++currentBatchOfDepot[depot];
			currentBatchOfDepot[depot] %= params.batch.nbBatches;
			return false;
		}
	}
	return true;

}

bool MonoidPricing::priceBatch(int batch_l) {

#ifdef DEBUG_RES
	SolutionChecker checker;
#endif

	auto t0 = clock();
	// if (!isInitialized) init();
	// else 

	updateProblem(depot, batch_l);

	auto t1 = clock();
	routes.clear();

//	for (int depotId = 0; depotId < (int)data.depots.size(); ++depotId){
	// EvspProblem & pb = problems[depot];
	ProblemWithInfo & pbWithInfo = problems[getDepotBatchProblem(depot, batch_l)];
	EvspProblem & pb = pbWithInfo.problem;
	pb.setLabelMapMaxSize(params.cg.label_map_max_size);
	pb.setNSolutionsSearched(params.cg.max_routes);
	pb.setApproximatePricing(false);
	pb.setIsStopBoundActivated(false);
	if (params.cg.discardUsingDominance) pb.setLabelAlgorithm(EvspProblem::LabelAlgorithm::LabelCorrecting_withLabelMapMaxSize);
	else pb.setLabelAlgorithm(EvspProblem::LabelAlgorithm::LabelSetting_withLabelMapMaxSize);
//	pb.setLabelMapMaxSize(20000000);
	pb.setBoundsType(EvspProblem::Bounds::Single_Bound);

	if (approximatePricer) {
		pb.setIsMemoryBounded(true);
		pb.setMemoryBoundedMaxSize(params.cg.memoryBoundedMaxSize);
	}

	if (priceAllRoutesSmallerThanGap){
		pb.setLabelAlgorithm(EvspProblem::LabelAlgorithm::LabelSetting_withLabelMapMaxSize);
		ResourceEvspFunctions::setMaximumCost(gap);
		pb.setNSolutionsSearched(100000);
	}

	if (params.cg.bounds == Parameters::Single) {
		pb.solve(); 
		if (!pb.getLabelResult().isSolvedToOptimal){
			cerr << "Pricing not solved to optimality" << endl;
			throw;
		}
	}
	else if (params.cg.bounds == Parameters::Clustered) {
		pb.setBoundsType(EvspProblem::Bounds::Clustered_Bounds);
		pb.setNbClustersPerVertex(params.clus.nbClustersPerVertex);
		pb.solve();
		if (!pb.getLabelResult().isSolvedToOptimal){
			cerr << "Pricing not solved to optimality" << endl;
			throw;
		}
	}
	else if (params.cg.bounds == Parameters::Conditional) {
		pb.setBoundsType(EvspProblem::Bounds::Conditional_Bounds);
		pb.setConditionalDelta(params.cond.delta);
		pb.setConditionalUbCost(params.cond.ubCost);
		pb.solve();
		if (!pb.getLabelResult().isSolvedToOptimal){
			cerr << "Pricing not solved to optimality" << endl;
			throw;
		}
	}
	else if (params.cg.bounds == Parameters::SingleThenConditional) {
		if (pbWithInfo.previousOptimal){
			pb.solve();
			// previousOptimal[depot] = pb.getLabelResult().isSolvedToOptimal;
			pbWithInfo.previousOptimal = pb.getLabelResult().isSolvedToOptimal;
		}
		if (!pbWithInfo.previousOptimal){
			pb.setBoundsType(EvspProblem::Bounds::Conditional_Bounds);
			pb.setConditionalDelta(params.cond.delta);
			pb.setConditionalUbCost(params.cond.ubCost);
			pb.solve();
			if (!pb.getLabelResult().isSolvedToOptimal){
				cerr << "Pricing not solved to optimality" << endl;
				throw;
			}
		}
	} else {
		throw;
	}

	auto t2 = clock();
	pbWithInfo.previousOptimal = pb.getLabelResult().isSolvedToOptimal;
//		multimap<double, pair<list<string>, Resource > >
	auto solutions = pb.getSolutionsArcList();

	if (priceAllRoutesSmallerThanGap){
		ResourceEvspFunctions::setMaximumCost(std::numeric_limits<double>::infinity());
		if (solutions.size() == 100000){
			cerr << "unable to price all routes smaller than gap" << endl;
			throw;
		}
	}

	if (solutions.empty()) return false;

	pbWithInfo.previousValue = solutions.rbegin()->first;

	value = 0.0;
	bool negativeReducedCostFound = false;
	for (auto && path : solutions){
		if (path.first < -EPS || params.cg.isPositiveReducedCostRouteKept){
			if (path.first < -EPS) negativeReducedCostFound = true;
			Route * newRoute = new Route();
			newRoute->depot = depot;
			if (data.depots[depot].isArtificalDepotForVehiclesWithInitialCharge) newRoute->cost = data.vehicleWithInitialPosition_cost;
			else newRoute->cost = data.vehicle_cost;
			newRoute->red_cost = path.first;
			if (path.first - value < -EPS) value = path.first;
			for (auto && arc: path.second.first) {
				int arcId = stoi(arc);
#ifdef MULTI_CHARGE
				auto staArc = stationArcs[arcId];
				assert(arcId == staArc.id);
				newRoute->cost += staArc.cost;
				newRoute->vertices.insert(newRoute->vertices.end(), staArc.vertices.begin(), staArc.vertices.end());

#elif
// 				if (arcId < (int)data.edges.size()){
// 					newRoute->vertices.push_back(data.edges[arcId].from);
// //					out << "o " << data.edges[arcId].from << " d " << data.edges[arcId].to << " ";
// //					out << " " << data.edges[arcId].from << " ";
// 					newRoute->cost += data.edges[arcId].cost;
// 				} else {
// 					int baseArcId = arcId % data.edges.size();
// 					int stationId = arcId / data.edges.size();
// 					Edge const & fromToSta = data.edges[data.adj_matrix[data.edges[baseArcId].from][stationId]];
// 					Edge const & staToTo = data.edges[data.adj_matrix[stationId][data.edges[baseArcId].to]];
// //					out << "o " << data.edges[baseArcId].from << " s " << stationId << " d " << data.edges[baseArcId].to << " ";
// //					out << " " << data.edges[baseArcId].from << " s" << stationId << " ";
// 					newRoute->vertices.push_back(fromToSta.from);
// 					newRoute->vertices.push_back(fromToSta.to);
// 					newRoute->cost += fromToSta.cost + staToTo.cost;
// 				}
#endif
			}
			newRoute->vertices.push_back(depot);
#ifdef DEBUG_RES
			if (checker.run(newRoute)){
				auto logger = Console::getPtrLogger();
				auto res = pb.rebuildPathResource(path.second.first);
				logger->error("isFeasible: {}", ResourceEvspFunctions::isConstraintSatisfied(res));
				res = pb.rebuildPathResourceFromEnd(path.second.first);
				logger->error("isReverseFeasible: {}", ResourceEvspFunctions::isConstraintSatisfied(res));
			}
#endif
			routes.push_back(newRoute);
		}
		else {
			break;
		}

	}
#ifdef PROFILE_MRCSP
	auto t3 = clock();
	out << (t1-t0)/(double) CLOCKS_PER_SEC << " updating ,";
	out << (t2-t1)/(double) CLOCKS_PER_SEC << " solving ,";
	out << (t3-t2)/(double) CLOCKS_PER_SEC << " post-processing" << endl;
#endif

	return negativeReducedCostFound;
}
