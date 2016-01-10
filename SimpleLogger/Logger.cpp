#define __SIMPLE_LOGGER__

#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include "Clock.h"

FILE * Logger::log_file = nullptr;
Logger::_InnerSpinLock Logger::_spin_lock;

thread_local std::string Logger::_func = "";
thread_local int Logger::_line = 0;
thread_local Logger::LogLevel Logger::_logger_level = Logger::LogLevel::DEBUG;

Logger::Logger()
{
	if (log_file == nullptr)
	{
		errno_t err = fopen_s(&log_file, "logout.log", "a");
		if (err != 0)
		{
			log_file = nullptr;		//fail to open log file!
			//throw 
		}
	}
}

Logger::~Logger()
{

}

void Logger::debug(const char *format, ...) noexcept
{

}

Logger& Logger::debug(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::DEBUG;
	return *this;
}

void Logger::info(const char *format, ...) noexcept
{

}

Logger& Logger::info(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::INFO;
	return *this;
}

void Logger::error(const char *format, ...) noexcept
{

}

Logger& Logger::error(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::ERROR;
	return *this;
}

void Logger::operator() (const char *format, ...) noexcept
{
	if (log_file == nullptr)
	{
		return;
	}
	va_list _curosr;
	va_start(_curosr, format);
	int _length = strlen(format);
	for (const char *p = format; (p = strstr(p, "%s")) != nullptr; ++p)
	{
		char * str = va_arg(_curosr, char*);
		_length += strlen(str);
	}
	std::string _input(_length + 1, '\0');
	va_start(_curosr, format);
	vsnprintf((char*)_input.data(), _input.size(), format, _curosr);
	va_end(_curosr); 
	TimePoint time_point = TimePoint::builder().buildCurrentTimePoit();
	
	_spin_lock.lock();		//lock the output operation, because all threads share only one output-stream
	fprintf(log_file, _basic_format[static_cast<int>(_logger_level)], _func.c_str(), _line,
		time_point.year, time_point.month, time_point.day, time_point.hour, time_point.minuite,
		time_point.second, time_point.millisecond, time_point.microsecond, _input.c_str());
	fflush(log_file);
	_spin_lock.unlock();	//release lock
}
