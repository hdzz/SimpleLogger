#define __SIMPLE_LOGGER__

#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include "TimePoint.h"

thread_local std::string Logger::_func = "";
thread_local int Logger::_line = 0;
thread_local Logger::LogLevel Logger::_current_log_level = Logger::LogLevel::DEBUG;

std::map<std::string, std::pair<Logger::_InnerSpinLock *, std::atomic_int *>> Logger::_log_file_lock;
const size_t Logger::_pre_log_length = 100;

Logger::LogBuffer::LogBuffer(Logger &const logger, size_t size, size_t count) :
	threshold_size(size), threshold_count(count), outer(logger)
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
		buffer = new char[buffer_size = current_count + current_size + 1]{ '\0' };
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

	fprintf(outer._log_file, "%s", buffer);

	current_count = 0;
	current_size = 0;
}

const char * Logger::_basic_format[3] = {
	"[DEBUG][%s][func:%s][line:%d] ",
	"[INFO][%s][func:%s][line:%d] " ,
	"[ERROR][%s][func:%s][line:%d] "
};

//every log file can open only once
Logger::Logger(const std::string file_name, LogLevel base_level) : _buffer(*this, 8192, 64), _base_log_level(base_level), _log_file_name(file_name)
{
	if (_log_file_lock.find(_log_file_name) == _log_file_lock.end())
	{
		_log_file = fopen(_log_file_name.c_str(), "a");
		if (_log_file == nullptr)
		{
			//open file failed! throw exception
		}
		else
		{
			_log_file_lock[_log_file_name] = std::make_pair<_InnerSpinLock *, std::atomic_int *>(new _InnerSpinLock(), new std::atomic_int(1));
		}
	}
	else
	{
		_log_file_lock[_log_file_name].second->operator++(1);
	}
}

Logger::Logger(LogLevel base_level) : _log_file(stdout), _buffer(*this, 8192, 64), _base_log_level(base_level), _log_file_name("stdout")
{
	_log_file_lock[_log_file_name] = std::make_pair<_InnerSpinLock *, std::atomic_int *>(new _InnerSpinLock(), new std::atomic_int(1));
}

Logger::~Logger()
{
	_log_file_lock[_log_file_name].second->operator--(1);
	if (_log_file_lock[_log_file_name].second == 0)
	{
		if (_log_file_name != "stdout")
		{
			fclose(_log_file);
		}
		delete _log_file_lock[_log_file_name].first;
		delete _log_file_lock[_log_file_name].second;
		_log_file_lock.erase(_log_file_name);
	}
}

Logger& Logger::debug(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_current_log_level = LogLevel::DEBUG;
	return *this;
}

Logger& Logger::info(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_current_log_level = LogLevel::INFO;
	return *this;
}

Logger& Logger::error(const char * func, int line) noexcept
{
	_func = func;
	_line = line;
	_current_log_level = LogLevel::ERROR;
	return *this;
}

void Logger::operator() (bool is_flush_now, const char *format, ...) noexcept
{
	if (_log_file == nullptr || _current_log_level < _base_log_level)
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

	char *log = new char[_length + _pre_log_length]{ '\0' };

	va_start(_curosr, format);
	TimePoint time_point = TimePoint::getCurrentTimePoit();
	size_t restart_index = snprintf(log, _length + _pre_log_length, _basic_format[static_cast<int>(_current_log_level)],
		time_point.toString().c_str(), _func.c_str(), _line);
	restart_index += vsnprintf(log + restart_index, _length + _pre_log_length - restart_index, format, _curosr);
	va_end(_curosr);

	_log_file_lock[_log_file_name].first->lock();
	if (is_flush_now)
	{
		fprintf(_log_file, "%s\n", log);
		delete[]log;
	}
	else
	{
		_buffer.append(log, restart_index);
	}
	_log_file_lock[_log_file_name].first->unlock();
}
