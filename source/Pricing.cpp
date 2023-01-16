#include "Pricing.h"

bool Pricing::price(int depot, const Duals& duals, const Bounds& bounds, bool artificials)
{
	this->depot = depot;
	this->duals = &duals;
	this->bounds = &bounds;
	this->artificials = artificials;
	return do_price();
}
