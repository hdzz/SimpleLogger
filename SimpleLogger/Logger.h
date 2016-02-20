#pragma once
#include <string>
#include <atomic>

#ifndef __SIMPLE_LOGGER__H_
#define __SIMPLE_LOGGER__H_

/*
* simple logger, multithread supported
*/
class Logger final
{
public:
	Logger(const std::string file_name);

	Logger(decltype(stdout) out);

	~Logger();

	Logger& debug(bool is_flush_now, const char * func, int line) noexcept;

	Logger& info(bool is_flush_now, const char * func, int line) noexcept;

	Logger& error(bool is_flush_now, const char * func, int line) noexcept;

	void operator() (const char *, ...) noexcept;

private:

	enum class LogLevel : size_t
	{
		DEBUG = 0,
		INFO,
		ERROR
	};

	static const char * _basic_format[3];

	FILE* _log_file = nullptr;

	static const size_t	 pre_log_length;	//fix length of log header, which describes time ,functon and line of file

	static thread_local std::string _func;

	static thread_local int _line;

	static thread_local LogLevel _logger_level;

	static thread_local bool _is_flush_now;	//control weather output to a memert buffer first, false is not

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

	static _InnerSpinLock _spin_lock;

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
#define debugNonFlush debug(false, __func__, __LINE__)
#define debugFlush debug(true, __func__, __LINE__)
#define infoNonFlush info(false, __func__, __LINE__)
#define infoFlush info(true, __func__, __LINE__)
#define errorNonFlush error(false, __func__, __LINE__)
#define errorFlush error(true, __func__, __LINE__)
#endif

#endif