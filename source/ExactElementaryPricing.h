#ifndef EXACTELEMENTARYPRICING_H
#define EXACTELEMENTARYPRICING_H

#include "Pricing.h"

class ExactElementaryPricing : public Pricing
{
public:
	ExactElementaryPricing();

	void init();
	bool do_price() override;

	void buildCompletionBounds() override;

private:
	class Label
	{
	public:
		double red_cost;
		int last;
	};

	std::vector<Label> labels;
	std::vector<Label> closeds;
};

#endif
