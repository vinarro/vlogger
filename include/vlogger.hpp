#ifndef __V_LOGGER_H__
#define __V_LOGGER_H__

#include <cstring>
#include <sstream>

static std::string LIBV_LOG_FILE_NAME("libv.log");

class VLogger
{
public:
    VLogger(const char *type, const char *file, int line, const char *func);
    ~VLogger();

    template <typename T>
    VLogger &operator<<(const T &t)
    {
        m_osstream << t;
        return *this;
    }

private:
    std::ostringstream m_osstream;
};

inline const char *GetFileName(const char *path)
{
#ifdef _WIN32
    const char* lastSlash = std::strrchr(path, '\\');
    const char* lastForwardSlash = std::strrchr(path, '/');

    if (lastSlash && lastForwardSlash) {
        if (lastForwardSlash > lastSlash) {
            lastSlash = lastForwardSlash;
        }
    } else if (lastForwardSlash) {
        lastSlash = lastForwardSlash;
    }
#else
    const char *lastSlash = strrchr(path, '/');
#endif
    return lastSlash ? lastSlash + 1 : path;
}

#define LOG_INFO() VLogger("INFO", GetFileName(__FILE__), __LINE__, __FUNCTION__)
#define LOG_DEBUG() VLogger("DEBUG", GetFileName(__FILE__), __LINE__, __FUNCTION__)
#define LOG_WARN() VLogger("WARN", GetFileName(__FILE__), __LINE__, __FUNCTION__)
#define LOG_ERROR() VLogger("ERROR", GetFileName(__FILE__), __LINE__, __FUNCTION__)

#endif // __V_LOGGER_H__