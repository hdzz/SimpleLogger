#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>
#include <atomic>

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

	static thread_local std::string _func;

	static thread_local int _line;

	static thread_local LogLevel _logger_level;

	class _InnerSpinLock
	{
	public:
		void lock()
		{
			while (lck.test_and_set(std::memory_order_acquire))
			{
			}
		}

		void unlock()
		{
			lck.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag lck = ATOMIC_FLAG_INIT;
	};

	static _InnerSpinLock _spin_lock;

};

#ifndef __SIMPLE_LOGGER__
#define debug debug(__func__, __LINE__)
#endif