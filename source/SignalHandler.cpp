#include "SignalHandler.h"

#include <iostream>
#include <csignal>
#include <mutex> // Trying to avoid the multiple prints problem.

using namespace std;

SignalHandler* SignalHandler::handler = nullptr;

void setSignals();

void SignalHandler::setHandler(SignalHandler* instance)
{
	handler = instance;
	setSignals();
}

void handleOnce()
{
	if (SignalHandler::handler != nullptr)
		SignalHandler::handler->doCleanExit();
}

once_flag flag;
void handleSignal(int signum)
{
	cout << "Received signal " << signum << endl;
	call_once(flag, handleOnce);
	exit(-2);
}

void setSignals()
{
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);
}
