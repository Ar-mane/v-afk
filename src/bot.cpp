#include "bot.h"

#include "resource.h"

#include <chrono>
#include <random>
#include <string_view>
#include <thread>
#include <windows.h>

Bot::Bot() : m_running(false), m_lastSessionSeconds(0.f)
{
}

Bot::~Bot()
{
    Stop();
}

void Bot::Start()
{
    if (m_running.exchange(true))
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_startTime = std::chrono::steady_clock::now();
        m_lastSessionSeconds = 0.f;
        m_currentScenario.clear();
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
        m_lastSessionSeconds = static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count());
        m_currentScenario.clear();
    }
}

bool Bot::IsRunning() const
{
    return m_running.load();
}

void Bot::GetScenarioPreview(char* out, size_t outSize) const
{
    if (!out || outSize == 0)
    {
        return;
    }

    out[0] = '\0';

    std::lock_guard<std::mutex> lock(m_stateMutex);
    if (m_currentScenario.empty())
    {
        return;
    }

    const size_t len = m_currentScenario.size() < 20 ? m_currentScenario.size() : 20;
    const size_t copyLen = len < outSize - 1 ? len : outSize - 1;
    m_currentScenario.copy(out, copyLen);
    out[copyLen] = '\0';
}

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
    return static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(now - start).count());
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

    INPUT down = {};
    down.type = INPUT_KEYBOARD;
    down.ki.wVk = static_cast<WORD>(key);

    INPUT up = down;
    up.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, &down, sizeof(down));
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    SendInput(1, &up, sizeof(up));
}

void Bot::RunScenario(std::string_view pattern)
{
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_currentScenario.assign(pattern.begin(), pattern.end());
    }

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
