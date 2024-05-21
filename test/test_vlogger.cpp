#include "vlogger.hpp"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

using namespace std;
class test
{
public:
    test(int count, int start) : m_count(count), m_start(start), m_stop(true) {}
    ~test()
    {
        stop();
        join();
    }
    void run()
    {
        std::cout << "run!" << std::endl;
        int n = 0;
        int p;
        while (n < m_start) {
            m_m.lock();
            num++;
            p = num;
            // std::cout << num << std::endl;
            m_m.unlock();
            LOG_INFO() << "thread:" << m_count << " n:" << p;

            n++;
        }
        cout << "thread " << m_count << " end:" << n << endl;
    }
    void start(){
        m_stop = false;
        m_thread = std::thread(&test::run, this);
    };
    void stop() { m_stop = true; };
    void join()
    {
        if (m_thread.joinable()) m_thread.join();
    };

private:
    std::atomic<bool> m_stop;
    std::thread m_thread;
    static int num;
    static mutex m_m;
    int m_count;
    int m_start;
};

int test::num = 0;
mutex test::m_m;

int main()
{
    const int length = 10;
    test *arr[length];
    for (int i = 0; i < length; i++) {
        arr[i] = new test(i + 1, 5000);
    }

    for (int i = 0; i < length; i++) {
        arr[i]->start();
    }

    // std::this_thread::sleep_for(std::chrono::seconds(10));

    for (int i = 0; i < length; i++) {
        arr[i]->join();
    }
    cout << "return" << endl;
    return 0;
}