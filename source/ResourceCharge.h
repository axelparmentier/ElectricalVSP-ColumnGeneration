/*
 * ResourceCharge.h
 *
 *  Created on: Oct 3, 2018
 *      Author: axel
 */

#ifndef SOURCE_RESOURCECHARGE_H_
#define SOURCE_RESOURCECHARGE_H_

#include "ResourceEvsp.h"

class ResourceCharge {
public:
	friend class ResourceChargeFunctions;

	typedef enum {
		Neutral,
		Source,
		Sink,
		Standard
	} ChargeType ;

	ResourceCharge();
	ResourceCharge(ResourceCharge::ChargeType, double);

	static void setChargeFactor(double chargeFactor_l) {
		chargeFactor = chargeFactor_l;
	}

	static void setMaxDistance(double maxDistance_l) {
		maxDistance = maxDistance_l;
	}

	static void setMaxCharge(double maxCharge_l) {
		maxCharge = maxCharge_l;
	}

	ResourceEvsp getResourceEvsp() const;
    ResourceEvsp getResourceEvspArtificial() const;
	double getTotalDistance() const {
		return totalDistance;
	}

	//---------------------------------------------------
	// Mandatory methods to be used in the resource framework
	//---------------------------------------------------
	// Monoid element ------------
	void expand(const ResourceCharge &);

	// Lattice Element -----------
	void meet(const ResourceCharge &);
	void join(const ResourceCharge &);

	double distance(const ResourceCharge &) const;

	bool isLeq(const ResourceCharge &) const;
	bool isGeq(const ResourceCharge &) const;

	// Complete Lattice--
	void setToNeutral();
    void setToSupremum();
    void setToInfimum();


private:
	static double maxDistance;
	static double chargeFactor;
	static double maxCharge;

	ChargeType type_m;
	double minimalChargeBegin;
	double chargeUsedEnd;
	double totalDistance;

	double getMinimumChargeRequiredAtBegin() const;
	double getMaxChargeDoableAtTheEnd() const;
	double getAdditionalChargeIfUnderMaxCharge() const;
//	double getThresholdAboveWhichAdditionnalBeginChargeUnused() const;
//	double getAdditionalChargeAtTheEndIfUnderThreshold() const;

};

class ResourceChargeFunctions {
public:
	static double getCost(const ResourceCharge & r);
	static bool isConstraintSatisfied(const ResourceCharge & r);
	static double getResourceConstraintHeuristic(const ResourceCharge & r);
};

#endif /* SOURCE_RESOURCECHARGE_H_ */
