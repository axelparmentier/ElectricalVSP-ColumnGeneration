#ifndef ROUTEENUMERATION_H
#define ROUTEENUMERATION_H

#include "Pricing.h"

#include <limits>

class RouteEnumeration : public Pricing
{
public:
	RouteEnumeration();

	void init();
	bool do_price() override;

	void setGap(double value) { gap = value; }
	void setCompletionBounds(const std::vector<double>& value) { completion_bounds = value; }

private:
	class Label
	{
	public:
		Label(int task = -1, double red_cost = std::numeric_limits<double>::infinity(), Label* last = nullptr) : task(task), red_cost(red_cost), last(last) {}

		int task;
		double red_cost;
		Label* last;
	};

	double gap;
	std::vector<std::vector<Label>> labels;
	std::vector<Label> closeds;

	void buildRoutes();
};

#endif
