#include "vlogger.hpp"

#include <unistd.h>

#include <chrono>
#include <iomanip>

#include "vlogfilewriter.hpp"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <pthread.h>
#else
#error Unsupported platform
#endif

inline unsigned __int64 getThreadId()
{
#ifdef _WIN32
    DWORD _tid = GetCurrentThreadId();
#elif defined(__linux__)
    pthread_t _tid = pthread_self();
#else
#error Unsupported platform
#endif
    unsigned __int64 tid = _tid;
    return tid;
};

static VLogFileWriter logFileWriter;

std::string getCurrentTimeFormatted()
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为time_t对象
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // 获取毫秒部分
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // 创建一个ostringstream实例来存储格式化的时间
    std::ostringstream formatted_time;

    // 使用put_time来格式化时间，这里使用的格式是YYYY-MM-DD HH:MM:SS
    formatted_time << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    
    // 添加毫秒部分
    formatted_time << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

    return formatted_time.str();
}

VLogger::VLogger(const char *type, const char *file, int line, const char *func) : m_osstream()
{
    m_osstream << "[" << type << "][" << getCurrentTimeFormatted() << "][" << getpid() << ":" << getThreadId() << "][" << file << "|"
               << line << "|" << func << "]";
};
VLogger::~VLogger()
{
    m_osstream << std::endl;
    logFileWriter.write(m_osstream.str().c_str());
};
