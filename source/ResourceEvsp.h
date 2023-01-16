/*
 * ResourceEvsp.h
 *
 *  Created on: Aug 24, 2018
 *      Author: axel
 */

#ifndef SOURCE_RESOURCEEVSP_H_
#define SOURCE_RESOURCEEVSP_H_

class ResourceEvsp {
	friend class ResourceEvspCost;
	friend class ResourceEvspFeasibility;
	friend class ResourceEvspFeasibilityHeuristic;
	friend class ResourceEvspFunctions;
public:
	ResourceEvsp();
	ResourceEvsp(double reducedCost, \
			double minimumChargeRequiredAtBegin,\
			double maxChargeDoableAtTheEnd,\
			double additionalChargeIfUnderMaxCharge);
	void testAppartenanceToResourceSet();

	double getAdditionalChargeIfUnderMaxCharge() const {return additionalChargeIfUnderMaxCharge;}

	//---------------------------------------------------
	// Mandatory methods to be used in the resource framework
	//---------------------------------------------------
	// Monoid element ------------
	void expand(const ResourceEvsp &);

	// Lattice Element -----------
	void meet(const ResourceEvsp &);
	void join(const ResourceEvsp &);

	double distance(const ResourceEvsp &) const;

	bool isLeq(const ResourceEvsp &) const;
	bool isGeq(const ResourceEvsp &) const;

	// Complete Lattice--
	void setToNeutral();
    void setToSupremum();
    void setToInfimum();

    static void setMaxCharge(double m){maxCharge = m;}
    static void setApproximatePricing(bool b){approximatePricing = b;}

	void addToReducedCost(double rc) { reducedCost += rc; }
	void setReducedCost(double rc) { reducedCost = rc; }

private:
	double reducedCost; // normal order

	static bool approximatePricing;

    static double maxCharge;
    bool isInfeasible;
	double minimumChargeRequiredAtBegin; // normal order
	double maxChargeDoableAtTheEnd; // reversed order
	double additionalChargeIfUnderMaxCharge; //reversedOrder
};

class ResourceEvspCost {
public:
	static double getkey(ResourceEvsp const &);
};

class ResourceEvspFeasibility {
public:
	static bool getkey(ResourceEvsp const &);
};

class ResourceEvspFeasibilityHeuristic {
public:
	static double getkey(ResourceEvsp const &);
};

class ResourceEvspFunctions{
	static double maximumCost;
public:
	static double getCost(const ResourceEvsp & r);
	static bool isConstraintSatisfied(const ResourceEvsp & r);
	static double getResourceConstraintHeuristic(const ResourceEvsp & r);

	static void setMaximumCost(double d){maximumCost = d;}
};


#endif /* SOURCE_RESOURCEEVSP_H_ */
