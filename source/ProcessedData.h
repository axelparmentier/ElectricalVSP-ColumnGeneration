#pragma once

#include "Singleton.h"

#include <vector>

class Data;

class ProcessedData : public Singleton<ProcessedData>
{
	friend class Singleton<ProcessedData>;
public:

	std::vector<int> tasks;
	std::vector<int> stations;

	std::vector<std::vector<int>> top_order;

	std::vector<std::vector<int>> back_edges;
	std::vector<std::vector<int>> back_top_order;

private:
	const Data& data;

	ProcessedData();

	void buildLists();

	void buildTopologicalOrdering();
	void buildBackwardsInformation();
};
