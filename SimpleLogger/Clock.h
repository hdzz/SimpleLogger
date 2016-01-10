#pragma once
#include <chrono>
#include <ctime>

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

public:

	class builder final
	{
	public:

		inline TimePoint buildCurrentTimePoit()
		{
			using namespace std::chrono;	//namespace only in this function
			time_t timer  = time(nullptr);
			tm *timeinfo = timeinfo = localtime(&timer);
			microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
			long long count = ms.count();
			short microsecond = count % 1000;
			short millisecond = (count / 1000) % 1000;

			return TimePoint(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, 
				timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, millisecond, microsecond);
		}
	};

private:
	TimePoint(short y, short M, short d, short H, short m, short s, short mi, short mc) :
		year(y), month(M), day(d), hour(H), minuite(m), second(s), millisecond(mi), microsecond(mc)
	{

	}
};