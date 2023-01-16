#ifndef LOADER_H
#define LOADER_H

class Data;
class Parameters;

class Loader
{
public:
	Loader();
	virtual ~Loader() = default;

	virtual bool load() = 0;

protected:
	Data & data;
	const Parameters& params;

	void loadUBs();
};

#endif
