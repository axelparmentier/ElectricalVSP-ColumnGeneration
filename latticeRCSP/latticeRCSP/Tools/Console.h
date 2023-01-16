/*
 * Console.h
 *
 *  Created on: 30 mars 2017
 *      Author: axel
 */

#ifndef SRC_LATTICERCSP_TOOLS_CONSOLE_H_
#define SRC_LATTICERCSP_TOOLS_CONSOLE_H_

#include "spdlog/spdlog.h"

using namespace std;
using namespace spdlog;

namespace latticeRCSP{
class Console {
	static const string consoleName;
	static int consoleNb;
	static spdlog::level::level_enum level;
	static std::shared_ptr<logger> ptrLogger;

public:
	static std::shared_ptr<logger> getPtrLogger();
	static void setToConsoleLogger();
	static void setToFileLogger(string filename);
	static void setLevel(spdlog::level::level_enum);
};

}


#endif /* SRC_LATTICERCSP_TOOLS_CONSOLE_H_ */
