#pragma once

class Vertex
{
public:
	int id;

	double ready;
	double due;
	double duration;
	double consumption;
	double initialCharge;

	enum Type
	{
		Depot,
		Task,
		Station,
		Vehicle
	} type;

	std::vector<int> edges;

	// bool hasToBeCovered() const {return (type == Task or type == Vehicle);} // Could be used to make vehicle cover mandatory
};
