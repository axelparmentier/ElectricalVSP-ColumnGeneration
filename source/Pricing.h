#ifndef PRICING_H
#define PRICING_H

#include "InputUser.h"

#include <vector>

class Route;
class Duals;
class Bound;
class Bounds;

class Pricing : public InputUser
{
public:
	Pricing() : value(0.0) {}
	virtual ~Pricing() = default;

	bool price(int depot, const Duals& duals, const Bounds& bounds, bool artificials);
	virtual bool do_price() = 0;

	double getValue() const { return value; }

	const std::vector<Route*>& getRoutes() const { return routes; }
	bool getArtificials() const {return artificials;}

	std::vector<double>& getCompletionBounds() { return completion_bounds; }
	virtual void buildCompletionBounds() {}

protected:
	int depot;
	const Duals* duals;
	const Bounds* bounds;
	bool artificials;

	double value;
	std::vector<Route*> routes;

	std::vector<double> completion_bounds;
};

#endif
