#include "InputUser.h"

#include <iostream>

#include "Data.h"
#include "Parameters.h"
#include "ProcessedData.h"

using namespace std;

InputUser::InputUser() : data(Data::getInstance()), pd(ProcessedData::getInstance()), params(Parameters::getInstance()), out(&null)
{
	setSilent(is_silent = params.silent);
}

void InputUser::setSilent(bool value)
{
	out.rdbuf((is_silent = value) ? &null : cout.rdbuf());
}
