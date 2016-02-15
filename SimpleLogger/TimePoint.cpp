#include "TimePoint.h"

const char TimePoint::default_format[] = "%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d";

TimePoint TimePoint::getCurrentTimePoit()
{
	using namespace std::chrono;	//namespace only in this function
	time_t timer = time(nullptr);
	tm *timeinfo = localtime(&timer);
	microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
	long long count = ms.count();
	short microsecond = count % 1000;
	short millisecond = (count / 1000) % 1000;

	return TimePoint(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, millisecond, microsecond);
}

std::string TimePoint::toString()
{
	using namespace std;
	string ret(30, '\0');
	snprintf((char*)ret.data(), ret.size() - 1, default_format, year, month, day, hour, minuite, second, millisecond, microsecond);
	return ret;
}