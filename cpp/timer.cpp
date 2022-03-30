#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <string>
#include <list>

template <class T>
class Timer
{
public:
    Timer(int delay, int interval, const std::function<void(Timer<T> &)> &callback, T &data)
    : m_active(true), m_delay(delay), m_interval(interval), m_callback(callback), m_data(data),
    m_thread([this]() { run();})
    {
        m_thread.detach();
    }
    ~Timer()
    {
    }

    void run()
    {
        if (m_active)
        {
            if (m_delay)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(m_delay));

                if (m_active)
                {
                    m_callback(*this);
                }
            }

            if (m_interval)
            {
                while (m_active)
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(m_interval));

                    if (m_active)
                    {
                        m_callback(*this);
                    }
                }
            }
        }
    }

    void stop()
    {
        m_active = false;
    }

    T& data()
    {
        return m_data;
    }

private:
    int    m_active         = false;
    int    m_delay;
    int    m_interval;
    const std::function<void(Timer<T> &)> &m_callback;
    T                                     &m_data;
    std::thread                           m_thread;

};

std::list<Timer<std::string>>    timers;

int main(void)
{
    std::string    id1("timer1");
    std::string    id2("timer2");

    std::function<void(Timer<std::string> &)> timeout_handler = [](Timer<std::string> &timer)
    {
        timer.stop();
        std::cout << "timer " << timer.data() << " expired" << std::endl;
    };

    Timer<std::string>    timer1(1*1000, 0, timeout_handler, id1);
    Timer<std::string>    timer2(2*1000, 0, timeout_handler, id2);

    timers.emplace_back(timer1);
    timers.emplace_back(timer2);

    std::this_thread::sleep_for(std::chrono::microseconds(3*1000));

    return 0;
}

