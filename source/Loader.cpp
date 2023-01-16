#include "Loader.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "Data.h"
#include "Parameters.h"

using namespace std;

Loader::Loader() : data(Data::getInstance()), params(Parameters::getInstance()) {}

void Loader::loadUBs()
{
	ifstream in("ubs.txt", ios::in);
	if (!in.good()) return;

	if (!params.silent) cout << "Loading ub...";

	bool found = false;
	while (in.good())
	{
		string name;
		double ub;
		in >> name >> ub;

		if (name == data.name)
		{
			data.ub = ub;
			found = true;
			break;
		}
	}

	if (!params.silent)
		if (found) cout << " found! ub = " << setprecision(3) << fixed << data.ub << endl;
		else cout << " not found!" << endl;
	in.close();
}
