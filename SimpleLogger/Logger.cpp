#define __SIMPLE_LOGGER__

#include "Logger.h"
#include <cstdarg>
#include <cstdio>

FILE * Logger::log_file = nullptr;

Logger::Logger()
{
	if (log_file == nullptr)
	{
		log_file = fopen("logout.log", "a");
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
	fprintf(log_file, _basic_format[static_cast<int>(_logger_level)], _func.c_str(), _line, _input.c_str());
	fflush(log_file);
}