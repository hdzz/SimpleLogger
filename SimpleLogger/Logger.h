#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>
//single thread only!
class Logger final
{
public:
	Logger();

	~Logger();

	void debug(const char *, ...) noexcept;

	Logger& debug(const char * func, int line) noexcept;

	void info(const char *, ...) noexcept;

	Logger& info(const char * func, int line) noexcept;

	void error(const char *, ...) noexcept;

	Logger& error(const char * func, int line) noexcept;

	void operator() (const char *, ...) noexcept;

private:

	enum class LogLevel : size_t
	{
		DEBUG = 0,
		INFO,
		ERROR
	};

	const char * _basic_format[3] = { 
		"[debug][func:%s][line:%d][%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d] %s\n", 
		"[info][func:%s][line:%d][%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d] %s\n" ,
		"[error][func:%s][line:%d][%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d] %s\n" 
	};

	static FILE* log_file;

	std::string _func;

	int _line;

	LogLevel _logger_level;

	time_t rawtime;

	struct tm * timeinfo;
};

#ifndef __SIMPLE_LOGGER__
#define debug debug(__func__, __LINE__)
#endif