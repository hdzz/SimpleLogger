#pragma once
#include <chrono>
#include <ctime>
#include <string>

/*
* immutable time point
*/
struct TimePoint final
{
	short const year;

	short const month;

	short const day;

	short const hour;

	short const minuite;

	short const second;

	short const millisecond;

	short const microsecond;

	static const char default_format[40];
public:

	TimePoint static getCurrentTimePoit();

	std::string toString();
	
private:
	TimePoint(short y, short M, short d, short H, short m, short s, short mi, short mc) :
		year(y), month(M), day(d), hour(H), minuite(m), second(s), millisecond(mi), microsecond(mc)
	{

	}
};