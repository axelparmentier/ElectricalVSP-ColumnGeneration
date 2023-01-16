/*
 * ResourceCharge.cpp
 *
 *  Created on: Oct 3, 2018
 *      Author: axel
 */

#include "ResourceCharge.h"
#include <limits>
#include <algorithm>
#include <cassert>
#include <math.h>

#define EPS 1e-6

double ResourceCharge::maxDistance = std::numeric_limits<double>::infinity();
double ResourceCharge::maxCharge = std::numeric_limits<double>::infinity();
double ResourceCharge::chargeFactor = 0.8;

ResourceCharge::ResourceCharge() {
	setToNeutral();
}

ResourceCharge::ResourceCharge(ResourceCharge::ChargeType type_l, double totalDistance_l){
	if (type_l == Neutral){
		setToNeutral();
		return;
	}
	if (totalDistance_l > maxCharge) throw "not implemented -- such arcs should not be added ";
	type_m = type_l;
	totalDistance = totalDistance_l;
	minimalChargeBegin = 0;
	chargeUsedEnd = 0;
	if (type_l == Source) minimalChargeBegin = totalDistance;
	else if (type_l == Sink) chargeUsedEnd = totalDistance;
	else{
		assert(type_l == Standard);
	}
}

void ResourceCharge::expand(ResourceCharge const & r){
	if (r.type_m == Neutral) return;
	assert(type_m != Sink);
	if (type_m == Neutral){
		type_m = r.type_m;
		minimalChargeBegin = r.minimalChargeBegin;
		totalDistance = r.totalDistance;
		chargeUsedEnd = r.chargeUsedEnd;
		return;
	}
	assert(r.type_m != Source);
	type_m = Standard;
	totalDistance += r.totalDistance;
	chargeUsedEnd = r.chargeUsedEnd;
}

void ResourceCharge::meet(ResourceCharge const & r){
	if (r.type_m == Neutral) type_m = Neutral;
	if (r.minimalChargeBegin < minimalChargeBegin) minimalChargeBegin = r.minimalChargeBegin;
	if (r.totalDistance < totalDistance) totalDistance = r.totalDistance;
	if (r.chargeUsedEnd < chargeUsedEnd) chargeUsedEnd = r.chargeUsedEnd;
}

void ResourceCharge::join(ResourceCharge const & r){
	if (r.minimalChargeBegin > minimalChargeBegin) minimalChargeBegin = r.minimalChargeBegin;
	if (r.totalDistance > totalDistance) totalDistance = r.totalDistance;
	if (r.chargeUsedEnd > chargeUsedEnd) chargeUsedEnd = r.chargeUsedEnd;
}

double ResourceCharge::distance(ResourceCharge const & r) const{
	return fabs(totalDistance - r.totalDistance) + fabs(minimalChargeBegin - r.minimalChargeBegin) + fabs(chargeUsedEnd - r.chargeUsedEnd);
}

bool ResourceCharge::isLeq(const ResourceCharge & r) const{
	if (minimalChargeBegin > r.minimalChargeBegin + EPS) return false;
	if (totalDistance > r.totalDistance + EPS) return false;
	if (chargeUsedEnd > r.chargeUsedEnd + EPS) return false;
	return true;
}

bool ResourceCharge::isGeq(ResourceCharge const & r) const {
	return r.isLeq(*this);
}

void ResourceCharge::setToNeutral() {
	type_m = Neutral;
	minimalChargeBegin = 0;
	totalDistance = 0;
	chargeUsedEnd = 0;
}

void ResourceCharge::setToSupremum() {
	type_m = Standard;
	minimalChargeBegin = std::numeric_limits<double>::infinity();
	totalDistance = std::numeric_limits<double>::infinity();
	chargeUsedEnd = std::numeric_limits<double>::infinity();
}

void ResourceCharge::setToInfimum() {
	setToNeutral();
}

ResourceEvsp ResourceCharge::getResourceEvsp() const{
	return ResourceEvsp(totalDistance, getMinimumChargeRequiredAtBegin(), \
			getMaxChargeDoableAtTheEnd(), \
			getAdditionalChargeIfUnderMaxCharge());
}

ResourceEvsp ResourceCharge::getResourceEvspArtificial() const{
	return ResourceEvsp(0, getMinimumChargeRequiredAtBegin(), \
			getMaxChargeDoableAtTheEnd(), \
			getAdditionalChargeIfUnderMaxCharge());
}

double ResourceCharge::getMinimumChargeRequiredAtBegin() const{
	double chargeAvailable = (maxDistance - totalDistance)/ chargeFactor;
	assert(chargeAvailable > -EPS);
	return std::max(minimalChargeBegin, totalDistance - chargeAvailable);
}

double ResourceCharge::getMaxChargeDoableAtTheEnd() const {
	double chargeAvailable = (maxDistance - totalDistance)/ chargeFactor;
	assert(chargeAvailable > -EPS);
	double initialMax = maxCharge;
	double consumptionToLastStation = totalDistance - chargeUsedEnd;
	double chargeLastStation_nolimit = initialMax + chargeAvailable - consumptionToLastStation;
	return std::min(chargeLastStation_nolimit, maxCharge) - chargeUsedEnd;
}

double ResourceCharge::getAdditionalChargeIfUnderMaxCharge() const{
	double chargeAvailable = (maxDistance - totalDistance)/ chargeFactor;
	assert(chargeAvailable > -EPS);
	double chargeLastStationIfMinimumChargeAtBegin_nolimit = getMinimumChargeRequiredAtBegin() + chargeAvailable - (totalDistance - chargeUsedEnd);
	double chargeAtTheEndIfMinimumChargeAtBegin = std::min(maxCharge, chargeLastStationIfMinimumChargeAtBegin_nolimit) - chargeUsedEnd;
	return chargeAtTheEndIfMinimumChargeAtBegin - getMinimumChargeRequiredAtBegin();
}

//
//double ResourceCharge::getThresholdAboveWhichAdditionnalBeginChargeUnused() const{
//	double chargeAvailable = (maxDistance - totalDistance)/ chargeFactor;
//	double chargeAtTheEndIfMinimumChargeAtBegin = std::min(maxCharge, getMinimumChargeRequiredAtBegin() + chargeAvailable - totalDistance) - chargeUsedEnd;
//	return std::min(maxCharge, getMinimumChargeRequiredAtBegin() + ((maxCharge - chargeUsedEnd) - chargeAtTheEndIfMinimumChargeAtBegin));
//}
//
//double ResourceCharge::getAdditionalChargeAtTheEndIfUnderThreshold() const {
//	double chargeAvailable = (maxDistance - totalDistance)/ chargeFactor;
//	double chargeAtTheEndIfMinimumChargeAtBegin = std::min(maxCharge, getMinimumChargeRequiredAtBegin() + chargeAvailable - totalDistance) - chargeUsedEnd;
//	return chargeAtTheEndIfMinimumChargeAtBegin - getMinimumChargeRequiredAtBegin();
//}

double ResourceChargeFunctions::getCost(ResourceCharge const & r){
	return r.totalDistance;
}

bool ResourceChargeFunctions::isConstraintSatisfied(ResourceCharge const & r){
	double chargeTimeNeeded = (r.totalDistance - ResourceCharge::maxCharge) * ResourceCharge::chargeFactor;
	double chargeTimeAvailable = ResourceCharge::maxDistance - r.totalDistance;
	if (chargeTimeAvailable < -EPS) return false;
	return chargeTimeNeeded < chargeTimeAvailable + EPS;
}

double ResourceChargeFunctions::getResourceConstraintHeuristic(ResourceCharge const & r){
	double chargeTimeNeeded = (r.totalDistance - ResourceCharge::maxCharge) * ResourceCharge::chargeFactor;
	double chargeTimeAvailable = ResourceCharge::maxDistance - r.totalDistance;
	return chargeTimeNeeded + chargeTimeAvailable;
}

