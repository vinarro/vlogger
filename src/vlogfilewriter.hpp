#ifndef __SCP_LOGFILEWRITER_H__
#define __SCP_LOGFILEWRITER_H__

#include <atomic>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class VLogFileWriter
{
public:
    VLogFileWriter();
    VLogFileWriter(std::string &logFilePath);
    ~VLogFileWriter();
    inline void write(const char *str)
    {
        size_t length = std::strlen(str) + 1;
        char *temp = (char *)malloc(length * sizeof(char));
        memcpy(temp, str, length);
        m_mutex.lock();
        m_queue.push(temp);
        m_mutex.unlock();
    }

private:
    void run();

private:
    std::atomic<bool> m_stop;
    std::thread m_thread;
    std::queue<const char *> m_queue;
    std::mutex m_mutex;
    std::filesystem::path *mp_logFilePath;
    std::ofstream m_ofstream;
};

#endif // __SCP_LOGFILEWRITER_H__