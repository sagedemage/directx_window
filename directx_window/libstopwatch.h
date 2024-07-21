#pragma once

/* Standard Libraries */
#include <chrono>

class StopWatch {
private:
	/* Global Declarations */
	std::chrono::steady_clock::time_point start, end;
public:
	/* Timer Methods */
	void startTimer();
	void endTimer();
	void printTime(std::string description);
};