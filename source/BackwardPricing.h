#ifndef BACKWARDPRICING_H
#define BACKWARDPRICING_H

#include "Pricing.h"

class BackwardPricing : public Pricing
{
public:
	BackwardPricing();

	void init();
	bool do_price() override;

	void buildCompletionBounds() override;

private:
	class Label
	{
	public:
		double red_cost;
		int next;
	};

	std::vector<Label> labels;
	std::vector<Label> closeds;
};

#endif
