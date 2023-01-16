#ifndef INPUTUSER_H
#define INPUTUSER_H

#include <ostream>

class Data;
class Parameters;
class ProcessedData;

class InputUser
{
public:
	InputUser();
	virtual ~InputUser() {}

	void setSilent(bool value);

protected:
	const Data& data;
	const Parameters& params;
	const ProcessedData& pd;

	bool is_silent;
	std::ostream out;

private:
	class NullBuffer : public std::streambuf
	{
	public:
		int overflow(int c) override { return c; }
	};
	NullBuffer null;
};

#endif
