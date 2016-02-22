#include "Logger.h"
#include "TimePoint.h"
#include <thread>
#include <sstream>
#include <iostream>

using namespace std;

void thread_func_xyb(Logger &logger, string id)
{
	for (int i = 0; i < 50; ++i)
	{
		logger.debug(false, "this is from thread %s, %s %4d %.2f", id.c_str(), "xyb", 520, 13.14F);
	}
}

void thread_func_love(Logger &logger, string id)
{
	for (int i = 0; i < 50; ++i)
	{
		logger.info(false, "this is from thread %s, %s %4d %.2f", id.c_str(), "love", 520, 13.14F);
	}
}

void thread_func_Charlotte(Logger &logger, string id)
{
	for (int i = 0; i < 50; ++i)
	{
		logger.error(true, "this is from thread %s, %s %4d %.2f", id.c_str(), "Charlotte", 520, 13.14F);
	}
}

int main()
{
	Logger logger_logout("logout.log", Logger::LogLevel::INFO);
	Logger logger("test.log", Logger::LogLevel::DEBUG);
	Logger logger1("test.log", Logger::LogLevel::DEBUG);
	Logger logger2("test.log", Logger::LogLevel::DEBUG);
	Logger logger_stdout(Logger::LogLevel::DEBUG);

	thread test0(thread_func_xyb, std::ref(logger_logout), "00");
	thread test1(thread_func_love, std::ref(logger_logout), "01");
	thread test2(thread_func_Charlotte, std::ref(logger), "02");
	thread test3(thread_func_Charlotte, std::ref(logger1), "03");
	thread test4(thread_func_Charlotte, std::ref(logger_stdout), "04");
	test0.join();
	test1.join();
	test2.join();
	test3.join();
	test4.join();

	return 0;
}