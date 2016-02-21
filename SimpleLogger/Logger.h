#pragma once
#ifndef __SIMPLE_LOGGER__H_
#define __SIMPLE_LOGGER__H_

#include <string>
#include <atomic>
#include <memory>
#include <map>

/*
* simple logger, multithread supported
*/
class Logger final
{
public:
	enum class LogLevel : size_t
	{
		DEBUG = 0,
		INFO,
		ERROR
	};

public:
	//init log to a file
	Logger(const std::string file_name, LogLevel base_level);

	//init log to stdout
	Logger(LogLevel base_level);

	~Logger();

	Logger& debug(const char * func, int line) noexcept;

	Logger& info(const char * func, int line) noexcept;

	Logger& error(const char * func, int line) noexcept;

	/*
	* format log infomation, formatting rule is the same with printf
	* is_flush_now: indicates whether log to a file immediately or log to a buffer then batch log to a file
	*/
	void operator() (bool is_flush_now, const char *format, ...) noexcept;

private:

	/*
	* const variables
	*/
	static const char * _basic_format[3];

	//length of log header, which describes time ,functon and line of file
	static const size_t	 _pre_log_length;

	/*
	* thread_local variables
	*/
	static thread_local std::string _func;

	static thread_local int _line;

	static thread_local LogLevel _current_log_level;


	/*
	* simple spin lock
	*/
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

	/*
	* normal member variables
	*/
	std::string _log_file_name;

	FILE* _log_file = nullptr;

	LogLevel _base_log_level = LogLevel::DEBUG;

	//every log flie holds a spin lock. count times of open every log file
	static std::map<std::string, std::tuple<FILE*, _InnerSpinLock *, std::atomic_int *>> _log_file_lock;

	/*
	* LogBuffer store log, when reach max couunt or max size
	* automaticly flush
	*/
	class LogBuffer final
	{
	public:
		LogBuffer(Logger &const logger, size_t size = 8192, size_t count = 128);

		virtual ~LogBuffer();

		/*input is the string, length is not include last '\0', just the string length*/
		void append(char * input, size_t lenght);

		void flush();

	private:
		struct LogRecord final
		{
			char * record = nullptr;
			size_t length = 0;

			void clear()
			{
				delete[] record;
				record = nullptr;
				length = 0;
			}

			~LogRecord()
			{
				delete[]record;
			}
		};

	private:
		size_t threshold_size;	//max buffer size

		size_t threshold_count;	//max log count

		size_t current_size = 0;

		size_t current_count = 0;

		LogRecord *log_array;

		size_t buffer_size;

		char *buffer;

		Logger &outer;

	};

	friend class LogBuffer;

	LogBuffer _buffer;

};

#ifndef __SIMPLE_LOGGER__
#define debug debug(__func__, __LINE__)
#define info info(__func__, __LINE__)
#define error error(__func__, __LINE__)
#endif

#endif