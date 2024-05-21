#include "vlogfilewriter.hpp"

#include "vlogger.hpp"

#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <windows.h>

size_t getFileSize(const char *fileName)
{
    if (fileName == NULL) {
        return 0;
    }
    struct stat statbuf;
    stat(fileName, &statbuf);
    size_t filesize = statbuf.st_size;
    return filesize;
}

#define PATH_BUFFER_MAX_SIZE (1024 * 1024)
bool GetMyselfAbsolutePath(wchar_t **p_buffer, size_t *p_size)
{
    size_t buffer_size = *p_size;
    wchar_t *buffer = *p_buffer;
    if (buffer_size > PATH_BUFFER_MAX_SIZE) {
        buffer_size = 256;
    } else if (buffer_size < 1) {
        buffer_size = 256;
    }
    do {
        if (buffer == nullptr) {
            buffer = (wchar_t *)malloc(sizeof(wchar_t) * buffer_size);
        }
        if (buffer == nullptr) {
            return false;
        }
        DWORD length = GetModuleFileNameW(NULL, buffer, buffer_size);
        DWORD err = GetLastError();
        if (err == ERROR_SUCCESS) {
            // buffer[length] = '\0';
            *p_buffer = buffer;
            *p_size = length;
            return true;
        } else if (err == ERROR_INSUFFICIENT_BUFFER) {
            free(buffer);
            buffer = nullptr;
            buffer_size *= 2;
            if (buffer_size > PATH_BUFFER_MAX_SIZE) {
                return false;
            }
            continue;
        } else {
            return false;
        }
    } while (true);
    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
    return false;
}

std::string GetMyselfFolderAbsolutePath()
{
    std::string folderPathString;
    wchar_t *path = nullptr;
    size_t length = 0;
    if (!GetMyselfAbsolutePath(&path, &length)) {
        return folderPathString;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    std::filesystem::path folderPath(converter.to_bytes(path));
    return folderPath.parent_path().string();
}

VLogFileWriter::VLogFileWriter()
{
    m_stop = false;
    std::string folderPathString;
    wchar_t *filePath = nullptr;
    size_t length = 0;
    if (!GetMyselfAbsolutePath(&filePath, &length)) {
        mp_logFilePath = nullptr;
        return;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    mp_logFilePath = new std::filesystem::path(converter.to_bytes(filePath));
    mp_logFilePath->replace_filename(LIBV_LOG_FILE_NAME);
    m_ofstream.open(*mp_logFilePath, std::ios::app);
    m_ofstream << "start!" << std::endl;
    m_thread = std::thread(&VLogFileWriter::run, this);
};

VLogFileWriter::VLogFileWriter(std::string &logFilePath)
{
    m_stop = false;
    mp_logFilePath = new std::filesystem::path(logFilePath);
    m_ofstream.open(*mp_logFilePath, std::ios::app);
    m_ofstream << "start!" << std::endl;
    m_thread = std::thread(&VLogFileWriter::run, this);
};

VLogFileWriter::~VLogFileWriter()
{
    m_stop = true;
    if (m_thread.joinable()) {
        m_thread.join();
    }
    if (m_ofstream.is_open()) {
        m_ofstream.close();
    }
};

void VLogFileWriter::run()
{
    while (true) {
        m_mutex.lock();
        if (m_queue.empty()) {
            m_mutex.unlock();
            if (m_stop)
                break;
            else
                continue;
        }
        const char *str = m_queue.front();
        m_queue.pop();
        m_mutex.unlock();
        if (m_ofstream.is_open() && !m_ofstream.good()) {
            m_ofstream.close();
        }
        int retryCount = 0;
        while (!m_ofstream.is_open()) {
            m_ofstream.open(*mp_logFilePath, std::ios::app);
            retryCount++;
            if (retryCount > 5) {
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        if (m_ofstream.good()) {
            m_ofstream << str;
            m_ofstream.flush();
        }
    }
}
