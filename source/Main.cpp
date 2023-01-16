#include <iostream>

#include "Data.h"
#include "Parameters.h"

#include "VSPLoader.h"
#include "EVSPLoader.h"

#include "ColumnGeneration.h"
#include "BranchAndBound.h"
#include "SolutionChecker.h"
#include "SimplePricingRunner.h"

int main(int argc, char* argv[])
{
	Parameters& params = Parameters::getInstance();
	if (!params.load(argc, argv)) return -1;

	Loader* loader = nullptr;
	switch (params.prob_type)
	{
	case Parameters::VSP: loader = new VSPLoader(); break;
	case Parameters::EVSP: loader = new EVSPLoader(); break;
	default: return -1;
	}

	if (!loader->load()) return -1;
	delete loader;

	Algorithm* algo = nullptr;
	switch (params.algo_type)
	{
	case Parameters::ColumnGeneration: algo = new ColumnGeneration(); break;
	case Parameters::BranchAndPrice: algo = new BranchAndBound(); break;
	case Parameters::SolutionFileChecker: algo = new SolutionChecker(); break;
	case Parameters::SimplePricingRunner: algo = new SimplePricingRunner(); break;
	default: return -1;
	}

	if (algo != nullptr)
	{
		if (algo->run())
		{

		}
		delete algo;
	}

	return 0;
}
