/*
 * Console.cpp
 *
 *  Created on: 30 mars 2017
 *      Author: axel
 */

#include "Console.h"

namespace latticeRCSP{

std::shared_ptr<logger> Console::ptrLogger = NULL;
int Console::consoleNb = 0;
const string Console::consoleName = "latticeRCSPconsole";
spdlog::level::level_enum Console::level = spdlog::level::info;


void Console::setToConsoleLogger(){
	consoleNb++;
	ptrLogger = spdlog::stdout_color_mt(consoleName + to_string(consoleNb));
	ptrLogger->set_level(level);
}

std::shared_ptr<logger> Console::getPtrLogger(){
	if (ptrLogger == 0){
		setToConsoleLogger();
	}
	return ptrLogger;
}

void Console::setToFileLogger(string filename){
	ptrLogger = spdlog::basic_logger_mt(consoleName + to_string(consoleNb), filename);
	ptrLogger->set_level(level);
}

void Console::setLevel(level::level_enum lev){
	level = lev;
	if (ptrLogger == 0){
		setToConsoleLogger();
	}
	ptrLogger->set_level(level);
}


}

