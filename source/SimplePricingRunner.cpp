#include "SimplePricingRunner.h"

#include <fstream>
#include <sstream>

#include "Data.h"
#include "Parameters.h"

#include "MonoidPricing.h"
#include "ExactElementaryPricing.h"

#include "BranchAndPriceStructs.h"

using namespace std;

bool SimplePricingRunner::run()
{
	if (params.simple.index == -1)
	{
		out << "File index not set! Aborting..." << endl;
		return false;
	}

	stringstream ss;
	ss << "out/" << data.name << "-" << params.simple.index << "-dual.txt";

	ifstream in(ss.str(), ios::in);
	if (!in.good())
	{
		out << "File " << ss.str() << " not found!" << endl;
		return false;
	}

	Duals duals;
	duals.vertices.resize(data.vertices.size());
	for (int v = 0; v < (int)data.vertices.size(); v++)
		in >> duals.vertices[v];

	in.close();

	Pricing* pricing;
	switch (params.cg.pricing)
	{
	case Parameters::Exact: pricing = new ExactElementaryPricing(); break;
	case Parameters::Monoid: pricing = new MonoidPricing(); break;
	default: return false;
	}

	Bounds bounds;
	for (int depot = 0; depot < (int)data.depots.size(); depot++)
		if (pricing->price(depot, duals, bounds, false))
			out << "Found " << pricing->getRoutes().size() << " routes for depot " << depot << "!" << endl;
		else
			out << "No route found for depot " << depot << "!" << endl;

	return true;
}
