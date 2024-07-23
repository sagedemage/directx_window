#ifndef LIBSTOPWATCH_H
#define LIBSTOPWATCH_H

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

#endif // LIBSTOPWATCH_H