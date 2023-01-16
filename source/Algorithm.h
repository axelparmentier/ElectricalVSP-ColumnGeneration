#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "InputUser.h"

class Algorithm : public InputUser
{
public:
	virtual ~Algorithm() = default;

	virtual bool run() = 0;
};

#endif
