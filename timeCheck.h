#pragma once
#include <iostream>
#include <chrono>

template<typename F>
double timeCheck(F&& f, int repeat = 1)
{
	volatile long long avoidOpt{}; // To avoid optimization 

	auto t1 = std::chrono::steady_clock::now();
	for (int i = 0; i < repeat; ++i)
	{
		avoidOpt += f(); 
	}
	auto t2 = std::chrono::steady_clock::now();

	(void)avoidOpt;
	return std::chrono::duration<double, std::milli>(t2 - t1).count() / repeat;
}

// For functions that return void
template<typename F>
double timeCheckVoid(F&& f, int repeat = 1)
{
	volatile int avoidOpt = 0;

	auto t1 = std::chrono::steady_clock::now();
	for (int i = 0; i < repeat; ++i)
	{
		f();
		avoidOpt++;
	}
	auto t2 = std::chrono::steady_clock::now();

	(void)avoidOpt;
	return std::chrono::duration<double, std::milli>(t2 - t1).count() / repeat;
}
