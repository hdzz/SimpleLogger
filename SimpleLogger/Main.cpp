#include "Logger.h"
#include "TimePoint.h"

int main()
{
	Logger logger;
	
	printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());
	for (int i = 0; i < 10; ++i)
	{
		logger.debugNonFlush("haha it's %s","xyb love Charlotte");
		logger.infoNonFlush("it's a simple: %s", "Charlotte love xyb");
		logger.errorNonFlush("love story: %s", "love make them live together and happy");
	}
	printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());

	printf("-----------------------------------------------------------\n");

	printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());
	for (int i = 0; i < 10; ++i)
	{
		logger.debugFlush("haha it's %s", "xyb love Charlotte");
		logger.infoFlush("it's a simple: %s", "Charlotte love xyb");
		logger.errorFlush("love story: %s", "love make them live together and happy");
	}
	printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());

	return 0;
}