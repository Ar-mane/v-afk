#include "bot.h"
#include "resource.h"

#include <windows.h>

#include <chrono>
#include <random>
#include <thread>

Bot::Bot()
{
    m_running = false;
    m_actions = 0;
    m_lastSessionSeconds = 0.0f;
}

Bot::~Bot() { Stop(); }

void Bot::Start()
{
    if (m_running.exchange(true))
    {
        return;
    }

    m_actions.store(0, std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_startTime = std::chrono::steady_clock::now();
        m_lastSessionSeconds = 0.0f;
    }

    m_thread = std::thread(&Bot::WorkerLoop, this);
}

void Bot::Stop()
{
    if (!m_running.exchange(false))
    {
        return;
    }

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    ReleaseAllKeys();

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        const auto now = std::chrono::steady_clock::now();
        m_lastSessionSeconds = static_cast<float>(
            std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count());
    }
}

bool Bot::IsRunning() const { return m_running.load(); }

int Bot::GetActions() const { return m_actions.load(std::memory_order_relaxed); }

float Bot::GetSessionTime() const
{
    if (!m_running.load())
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        return m_lastSessionSeconds;
    }

    std::chrono::steady_clock::time_point start;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        start = m_startTime;
    }

    const auto now = std::chrono::steady_clock::now();
    return static_cast<float>(
        std::chrono::duration_cast<std::chrono::seconds>(now - start).count());
}

int Bot::KeyFor(char symbol)
{
    switch (symbol)
    {
    case 'a':
        return 'A';
    case 'w':
        return 'W';
    case 'd':
        return 'D';
    case 's':
        return 'S';
    case 'c':
        return VK_SPACE;
    default:
        return 0;
    }
}

void Bot::ReleaseKey(int key)
{
    INPUT up = {};
    up.type = INPUT_KEYBOARD;
    up.ki.wVk = static_cast<WORD>(key);
    up.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &up, sizeof(INPUT));
}

void Bot::ReleaseAllKeys()
{
    ReleaseKey('A');
    ReleaseKey('W');
    ReleaseKey('D');
    ReleaseKey('S');
    ReleaseKey(VK_SPACE);
}

void Bot::HoldKey(int key, int durationMs)
{
    if (durationMs <= 0)
    {
        return;
    }

    const WORD vk = static_cast<WORD>(key);

    INPUT down = {};
    down.type = INPUT_KEYBOARD;
    down.ki.wVk = vk;
    down.ki.dwFlags = 0;

    INPUT up = down;
    up.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, &down, sizeof(down));
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    SendInput(1, &up, sizeof(up));

    m_actions.fetch_add(1, std::memory_order_relaxed);
}

void Bot::RunScenario(std::string_view pattern)
{
    for (std::size_t i = 0; i < pattern.size();)
    {
        if (!m_running.load())
        {
            ReleaseAllKeys();
            return;
        }

        const char symbol = pattern[i];
        std::size_t run = 1;

        while (i + run < pattern.size() && pattern[i + run] == symbol)
        {
            ++run;
        }

        const int key = KeyFor(symbol);
        if (key != 0)
        {
            HoldKey(key, static_cast<int>(run) * kMsPerChar);
        }

        i += run;
    }
}

void Bot::WorkerLoop()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::size_t> pick(0, kScenarios.size() - 1);

    while (m_running.load())
    {
        RunScenario(kScenarios[pick(rng)]);

        for (int i = 0; i < 50 && m_running.load(); ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
