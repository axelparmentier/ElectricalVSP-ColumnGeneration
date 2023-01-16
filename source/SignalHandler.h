#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

class SignalHandler
{
public:
	virtual ~SignalHandler() = default;
	virtual void doCleanExit() = 0;

	static SignalHandler* handler;
	static void setHandler(SignalHandler* instance);
};

#endif