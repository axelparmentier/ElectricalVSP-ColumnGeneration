/*
 * ResourceEvsp.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: axel
 */

#include "ResourceEvsp.h"
#include <algorithm>
#include <cassert>
#include <limits>
#include <math.h>
#define EPS 1e-6


double ResourceEvsp::maxCharge = std::numeric_limits<double>::infinity();
bool ResourceEvsp::approximatePricing = false;

void ResourceEvsp::testAppartenanceToResourceSet(){
	if (!isInfeasible){
		assert(minimumChargeRequiredAtBegin > -EPS);
		assert(minimumChargeRequiredAtBegin < maxCharge + EPS);
		assert(maxChargeDoableAtTheEnd > -EPS);
		assert(maxChargeDoableAtTheEnd < maxCharge + EPS);
		assert(additionalChargeIfUnderMaxCharge > -maxCharge -EPS);
		assert(additionalChargeIfUnderMaxCharge < maxCharge + EPS);
		assert(minimumChargeRequiredAtBegin + additionalChargeIfUnderMaxCharge < maxChargeDoableAtTheEnd + EPS);
		assert(maxCharge + additionalChargeIfUnderMaxCharge > maxChargeDoableAtTheEnd -EPS);
	}
}

ResourceEvsp::ResourceEvsp() {
	setToNeutral();
}

//double ResourceEvsp::chargeAtTheEndIfAboveThreshold() const{
//	return thresholdAboveWhichAdditionnalBeginChargeUnused + additionalChargeAtTheEndIfUnderThreshold;
//}

ResourceEvsp::ResourceEvsp(double reducedCost_l, \
		double minimumChargeRequiredAtBegin_l,\
		double maxChargeDoableAtTheEnd_l,\
		double additionalChargeIfUnderMaxCharge_l){
	reducedCost = reducedCost_l;
	isInfeasible = false;
	minimumChargeRequiredAtBegin = minimumChargeRequiredAtBegin_l;
	maxChargeDoableAtTheEnd = maxChargeDoableAtTheEnd_l;
	additionalChargeIfUnderMaxCharge = additionalChargeIfUnderMaxCharge_l;
	testAppartenanceToResourceSet();
}

void ResourceEvsp::expand(ResourceEvsp const & right){
	if (right.isInfeasible) isInfeasible = true;
	if (maxChargeDoableAtTheEnd < right.minimumChargeRequiredAtBegin - EPS) isInfeasible = true;
	if (isInfeasible) return;

	reducedCost += right.reducedCost;
	minimumChargeRequiredAtBegin = std::max(minimumChargeRequiredAtBegin, \
			 right.minimumChargeRequiredAtBegin - additionalChargeIfUnderMaxCharge);
	maxChargeDoableAtTheEnd = std::min(maxChargeDoableAtTheEnd + right.additionalChargeIfUnderMaxCharge, \
			right.maxChargeDoableAtTheEnd);
	additionalChargeIfUnderMaxCharge = std::min(additionalChargeIfUnderMaxCharge + right.additionalChargeIfUnderMaxCharge, \
			maxChargeDoableAtTheEnd - minimumChargeRequiredAtBegin);

	testAppartenanceToResourceSet();
}

void ResourceEvsp::meet(ResourceEvsp const & right){
	if (right.isInfeasible) return;
	if (isInfeasible){
		reducedCost = right.reducedCost;
		isInfeasible = false;
		minimumChargeRequiredAtBegin = right.minimumChargeRequiredAtBegin;
		maxChargeDoableAtTheEnd = right.maxChargeDoableAtTheEnd;
		additionalChargeIfUnderMaxCharge = right.additionalChargeIfUnderMaxCharge;
	} else{
		reducedCost = std::min(reducedCost,right.reducedCost);
		minimumChargeRequiredAtBegin = std::min(minimumChargeRequiredAtBegin, right.minimumChargeRequiredAtBegin);
		maxChargeDoableAtTheEnd = std::max(maxChargeDoableAtTheEnd, right.maxChargeDoableAtTheEnd);
		additionalChargeIfUnderMaxCharge = std::max(additionalChargeIfUnderMaxCharge, right.additionalChargeIfUnderMaxCharge);
	}
	testAppartenanceToResourceSet();
}

void ResourceEvsp::join(ResourceEvsp const & right){
	isInfeasible &= right.isInfeasible;
	if (isInfeasible) return;
	reducedCost = std::max(reducedCost,right.reducedCost);
	minimumChargeRequiredAtBegin = std::max(minimumChargeRequiredAtBegin, right.minimumChargeRequiredAtBegin);
	maxChargeDoableAtTheEnd = std::min(maxChargeDoableAtTheEnd, right.maxChargeDoableAtTheEnd);
	additionalChargeIfUnderMaxCharge = std::min(additionalChargeIfUnderMaxCharge, right.additionalChargeIfUnderMaxCharge);
	if (minimumChargeRequiredAtBegin + additionalChargeIfUnderMaxCharge > maxChargeDoableAtTheEnd + EPS) isInfeasible = true;
	testAppartenanceToResourceSet();
}

double ResourceEvsp::distance(const ResourceEvsp & right) const{
	if (isInfeasible != right.isInfeasible) return 10* maxCharge;
	double result = 0;
//	result += fabs(reducedCost - right.reducedCost);
	result += fabs(minimumChargeRequiredAtBegin - right.minimumChargeRequiredAtBegin);
	result += fabs(maxChargeDoableAtTheEnd - right.maxChargeDoableAtTheEnd);
	result += fabs(additionalChargeIfUnderMaxCharge - right.additionalChargeIfUnderMaxCharge);
	return result;
}

bool ResourceEvsp::isLeq(ResourceEvsp const & right) const{
	if (right.isInfeasible) return true;
	if (isInfeasible) return false;
	if (reducedCost > right.reducedCost + EPS) return false;
	if (!approximatePricing){
		if (minimumChargeRequiredAtBegin > right.minimumChargeRequiredAtBegin + EPS) return false;
		if (maxChargeDoableAtTheEnd < right.maxChargeDoableAtTheEnd + EPS) return false;
		if (additionalChargeIfUnderMaxCharge < right.additionalChargeIfUnderMaxCharge + EPS) return false;
	}
	return true;
}

bool ResourceEvsp::isGeq(ResourceEvsp const & right) const {
	return right.isLeq(*this);
}

void ResourceEvsp::setToNeutral(){
	isInfeasible = false;
	reducedCost = 0;
	minimumChargeRequiredAtBegin = 0;
	maxChargeDoableAtTheEnd = maxCharge;
	additionalChargeIfUnderMaxCharge = 0;
	testAppartenanceToResourceSet();
}

void ResourceEvsp::setToInfimum(){
	isInfeasible = false;
	reducedCost = 0;
	minimumChargeRequiredAtBegin = 0;
	maxChargeDoableAtTheEnd = maxCharge;
	additionalChargeIfUnderMaxCharge = maxCharge;
	testAppartenanceToResourceSet();
}

void ResourceEvsp::setToSupremum(){
	isInfeasible = true;
	testAppartenanceToResourceSet();
}

double ResourceEvspCost::getkey(const ResourceEvsp & r){
	return r.reducedCost;
}

bool ResourceEvspFeasibility::getkey(const ResourceEvsp & r){
	if (r.isInfeasible) return false;
	if (r.minimumChargeRequiredAtBegin > EPS) return false;
	return true;
}

double ResourceEvspFeasibilityHeuristic::getkey(const ResourceEvsp & r){
	return r.minimumChargeRequiredAtBegin - r.maxChargeDoableAtTheEnd - r.additionalChargeIfUnderMaxCharge;
}

double ResourceEvspFunctions::maximumCost = std::numeric_limits<double>::infinity();

double ResourceEvspFunctions::getCost(const ResourceEvsp & r){
	return r.reducedCost;
}

bool ResourceEvspFunctions::isConstraintSatisfied(const ResourceEvsp & r){
	if (r.isInfeasible) return false;
	if (r.minimumChargeRequiredAtBegin > EPS) return false;
	if (r.reducedCost > maximumCost) return false;
	return true;
}

double ResourceEvspFunctions::getResourceConstraintHeuristic(const ResourceEvsp & r){
	return r.minimumChargeRequiredAtBegin - r.maxChargeDoableAtTheEnd - r.additionalChargeIfUnderMaxCharge;
}
