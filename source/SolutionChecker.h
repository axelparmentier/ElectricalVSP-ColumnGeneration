#ifndef SOLUTIONFILECHECKER_H
#define SOLUTIONFILECHECKER_H

#include "Algorithm.h"

#include <vector>

class Route;

class SolutionChecker : public Algorithm
{
public:
	SolutionChecker();
	SolutionChecker(std::ostream& os);

	bool run() override;
	bool run(const std::vector<const Route*>& routes, double cost);
	bool run(const Route* route);

private:
	double cost;
	std::vector<const Route*> routes;

	std::ostream& os;

	bool check();
	bool loadFromFile();
};

#endif
