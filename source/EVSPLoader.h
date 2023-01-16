#ifndef EVSPLOADER_H
#define EVSPLOADER_H

#include "Loader.h"

class EVSPLoader : public Loader
{
public:
	bool load();

private:
	void cropInstance(int vehiclesWithInitialPosition);
};

#endif
