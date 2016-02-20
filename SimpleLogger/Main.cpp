#include "Logger.h"
#include "TimePoint.h"
#include <thread>
#include <sstream>
#include <iostream>

using namespace std;

void thread_func(Logger &logger, string id)
{
	for (int i = 0; i < 200; ++i)
	{
		if (id == "01")
		{
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		logger.infoFlush("this is from thread %s, %s %4d %.2f", id.c_str(), "xyb", 520, 13.14F);
	}
}

int main()
{
	//Logger logger("test.log");
	Logger logger(stdout);

	//cout << typeid(decltype(stdout)).name() << endl;
	//cout << typeid(FILE*).name() << endl;
	thread test0(thread_func, std::ref(logger), "00");
	thread test1(thread_func, std::ref(logger), "01");
	thread test2(thread_func, std::ref(logger), "02");
	test0.join();
	test1.join();
	test2.join();

	//printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());
	//for (int i = 0; i < 10; ++i)
	//{
	//	logger.debugNonFlush("haha it's %s","xyb love Charlotte");
	//	logger.infoNonFlush("it's a simple: %s", "Charlotte love xyb");
	//	logger.errorNonFlush("love story: %s", "love make them live together and happy");
	//}
	//printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());

	//printf("-----------------------------------------------------------\n");

	//printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());
	//for (int i = 0; i < 10; ++i)
	//{
	//	logger.debugFlush("haha it's %s", "xyb love Charlotte");
	//	logger.infoFlush("it's a simple: %s", "Charlotte love xyb");
	//	logger.errorFlush("love story: %s", "love make them live together and happy");
	//}
	//printf("%s\n", TimePoint::getCurrentTimePoit().toString().c_str());

	return 0;
}