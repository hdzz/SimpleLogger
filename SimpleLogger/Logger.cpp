#define __SIMPLE_LOGGER__

#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include "TimePoint.h"

FILE * Logger::log_file = nullptr;

thread_local std::string Logger::_func = "";
thread_local int Logger::_line = 0;
thread_local Logger::LogLevel Logger::_logger_level = Logger::LogLevel::DEBUG;
thread_local bool Logger::_is_flush_now = false;

Logger::_InnerSpinLock Logger::_spin_lock;
const size_t Logger::pre_log_length = 100;
Logger::LogBuffer Logger::_buffer(8192, 64);

Logger::LogBuffer::LogBuffer(size_t size, size_t count) :
	threshold_size(size), threshold_count(count)
{
	log_array = new LogRecord[threshold_count];
	buffer = new char[buffer_size = threshold_size << 1]{ '\0' };
}

Logger::LogBuffer::~LogBuffer()
{
	flush();
	delete[]log_array;
	delete[]buffer;
}

void Logger::LogBuffer::append(char * input, size_t lenght)
{
	log_array[current_count].record = input;
	log_array[current_count++].length = lenght;
	current_size += lenght;
	if (current_count >= threshold_count || current_count >= threshold_size)
	{
		flush();
	}
}

void Logger::LogBuffer::flush()
{
	if (1 + current_count + current_size > buffer_size)
	{
		delete[]buffer;
		buffer = new char[buffer_size = current_count + current_size]{ '\0' };
	}
	char *curse = buffer;
	for (size_t i = 0; i < current_count; ++i)
	{
		memcpy(curse, log_array[i].record, log_array[i].length + 1);
		curse += log_array[i].length + 1;
		*(curse - 1) = '\n';
		log_array[i].clear();
	}
	*curse = '\0';
	fprintf(log_file, "%s", buffer);
	current_count = 0;
	current_size = 0;
}

const char * Logger::_basic_format[3] = {
	"[DEBUG][%s][func:%s][line:%d] ",
	"[INFO][%s][func:%s][line:%d] " ,
	"[ERROR][%s][func:%s][line:%d] "
};

Logger::Logger(const std::string file_name)
{
	if (log_file == nullptr)
	{
		errno_t err = fopen_s(&log_file, file_name.c_str(), "a");
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

Logger& Logger::debug(bool is_flush_now, const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::DEBUG;
	_is_flush_now = is_flush_now;
	return *this;
}

Logger& Logger::info(bool is_flush_now, const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::INFO;
	_is_flush_now = is_flush_now;
	return *this;
}

Logger& Logger::error(bool is_flush_now, const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_logger_level = LogLevel::ERROR;
	_is_flush_now = is_flush_now;
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

	for (const char *p = format, *temp = nullptr; ; ++p)
	{
		if ((temp = strstr(p, "%s")) != nullptr)
		{
			char * str = va_arg(_curosr, char*);
			_length += strlen(str);
			p = temp + 1;
		}
		else if ((temp = strchr(p, '%')) != nullptr)
		{
			int number_int = va_arg(_curosr, int);
			_length += 25;
			p = temp;
		}

		if (temp == nullptr)
		{
			break;
		}
	}

	char *log = new char[_length + pre_log_length]{ '\0' };

	va_start(_curosr, format);
	TimePoint time_point = TimePoint::getCurrentTimePoit();
	size_t restart_index = snprintf(log, _length + pre_log_length, _basic_format[static_cast<int>(_logger_level)],
		time_point.toString().c_str(), _func.c_str(), _line);
	restart_index += vsnprintf(log + restart_index, _length + pre_log_length - restart_index, format, _curosr);
	va_end(_curosr);

	_spin_lock.lock();
	if (_is_flush_now)
	{
		fprintf(log_file, "%s\n", log);
		delete[]log;
	}
	else
	{
		_buffer.append(log, restart_index);
	}
	_spin_lock.unlock();
}
