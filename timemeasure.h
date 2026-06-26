#ifndef TIMEMEASURE_H
#define TIMEMEASURE_H

// For the clock (timer) library
#include <chrono>
// Stack for the clocks (timers)
#include <stack>

/* Class to use functions for measuring times equivalent
to MATLABS tic() toc() functions */
class MeasureTime {
	public:
		// Stack for the different timers
		std::stack<std::chrono::time_point<std::chrono::system_clock>> tictoc_stack;
		
		// Put new timer on the stack
		void tic();
		
		//Return passed time and remove last timer on stack
		double toc();
};

#endif
