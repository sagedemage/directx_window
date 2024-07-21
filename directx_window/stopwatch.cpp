/* Standard Libraries */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <comdef.h>
#include <exception>
#include <chrono>

/* Local header files */
#include "stopwatch.h"

void StopWatch::startTimer() {
	start = std::chrono::steady_clock::now();
}

void StopWatch::endTimer() {
	end = std::chrono::steady_clock::now();
}

void StopWatch::printTime(std::string description) {
	auto diff = end - start;
	double exe_time = std::chrono::duration<double, std::milli>(diff).count();
	std::string debug_msg = description + " execution time: " + std::to_string(exe_time) + "ms\n";
	OutputDebugStringA(debug_msg.c_str());
}