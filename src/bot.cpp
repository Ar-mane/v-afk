#include "bot.h"

#include <windows.h>

#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <vector>

Bot::Bot()
{
    m_running = false;
    m_actions = 0;
    m_lastSessionSeconds = 0.0f;
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

    m_actions.store(0, std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);

        m_startTime = std::chrono::steady_clock::now();

        m_lastSessionSeconds = 0.0f;
    }

    m_thread = std::thread(
        &Bot::WorkerLoop,
        this
    );
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

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);

        auto now = std::chrono::steady_clock::now();

        auto elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(
                now - m_startTime
            );

        m_lastSessionSeconds =
            static_cast<float>(elapsed.count());
    }
}

bool Bot::IsRunning() const
{
    return m_running.load();
}

int Bot::GetActions() const
{
    return m_actions.load(std::memory_order_relaxed);
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

    auto now = std::chrono::steady_clock::now();

    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(
            now - start
        );

    return static_cast<float>(elapsed.count());
}

void Bot::PressKey(
    int key,
    int durationMs
)
{
    INPUT input = {};

    input.type = INPUT_KEYBOARD;

    input.ki.wVk = key;

    SendInput(
        1,
        &input,
        sizeof(INPUT)
    );

    std::this_thread::sleep_for(
        std::chrono::milliseconds(durationMs)
    );

    input.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(
        1,
        &input,
        sizeof(INPUT)
    );

    m_actions.fetch_add(
        1,
        std::memory_order_relaxed
    );
}

void Bot::ExecuteAction(
    const Bot::Action& action
)
{
    switch (action.type)
    {
        case Bot::ActionType::MoveForward:
        {
            PressKey(
                'W',
                action.durationMs
            );

            break;
        }

        case Bot::ActionType::MoveBackward:
        {
            PressKey(
                'S',
                action.durationMs
            );

            break;
        }

        case Bot::ActionType::StrafeLeft:
        {
            PressKey(
                'A',
                action.durationMs
            );

            break;
        }

        case Bot::ActionType::StrafeRight:
        {
            PressKey(
                'D',
                action.durationMs
            );

            break;
        }

        case Bot::ActionType::Jump:
        {
            PressKey(
                VK_SPACE,
                action.durationMs
            );

            break;
        }

        case Bot::ActionType::Idle:
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(action.durationMs)
            );

            break;
        }
    }
}

std::vector<Bot::Action> Bot::GetRandomScenario(
    std::mt19937& rng
)
{
    static const std::vector<std::string> patterns =
    {
        "wwwwwwwwwwwwwwwwaadadadadadadadadddwwwwwwwwwwwwsssswwwwaadadadwwwwwwwwdddwwwwwwwwwwwwwwccwwwwwwaaadddwwwwwwwwww",
        "adadadadadadadadadadwwwwwwadadadadccwwwwddddaaaawwwwwwwwwwssswwwwwwadadadadadadwwwwwwwwccwwwwwwwwww",
        "wwwwwwwwwwwwaaaaaaaaaaaawwwwwwwwddddwwwwwwwwwwwwaadadadadwwwwccwwwwwwwwwwwwsssssswwwwddddddddwwwwwwwwww",
        "wwwwwwwwwwwwssssssssaaaaaaaawwwwddddddddwwwwwwwwwwadadadadadwwwwccwwwwwwsssswwwwwwwwwwwwaaaaddddwwwwwwww",
        "wwwwwwccwwwwwwccwwwwadadadadadadwwwwwwwwwwaaaaaaaaadddddddddwwwwwwccwwwwwwwwwwwwsssswwwwwwwwadadadadadadwwwwwwww",
        "wwwwwwwwwwwwwwwwwwaaaawwwwwwwwddddwwwwwwwwwwwwccwwwwwwadadadadwwwwwwwwwwwwsssswwwwaaaawwwwwwwwwwwwddddwwwwww",
        "wwwwwwwwaadadadwwwwssswwwwwwddddwwwwaaaaccwwwwwwwwwwadadadadadwwwwsssssswwwwwwwwwwwwddddaaaawwwwwwww",
        "aaaaaaaaddddddddadadadadadadadwwwwsssswwwwccwwwwwwwwwwaaaaaaaaddddddddwwwwwwadadadadwwwwwwwwwwwwsssswwwwwwww",
        "wwwwwwwwwwwwwwwwwwwwadadadadadadadadadwwwwwwwwsssssssswwwwwwwwwwaaaaddddaaaaddddwwwwwwwwccwwwwwwwwwwww",
        "sssswwwwsssswwwwadadadadadadadadccwwwwwwwwwwddddddddaaaaaaaawwwwwwwwwwwwwwwsssssssswwwwwwwwadadadad"
    };

    std::uniform_int_distribution<size_t> patternDist(
        0,
        patterns.size() - 1
    );

    const std::string& pattern =
        patterns[patternDist(rng)];

    std::uniform_int_distribution<int> sideTap(85, 155);
    std::uniform_int_distribution<int> forwardTap(100, 180);
    std::uniform_int_distribution<int> backTap(120, 230);
    std::uniform_int_distribution<int> jumpTap(55, 90);
    std::uniform_int_distribution<int> idleTap(35, 90);

    std::vector<Bot::Action> actions;
    actions.reserve(pattern.size() * 2);

    for (size_t i = 0; i < pattern.size();)
    {
        char symbol = pattern[i];
        size_t run = 1;

        while (i + run < pattern.size() && pattern[i + run] == symbol)
        {
            run++;
        }

        ActionType type = ActionType::Idle;
        int baseDuration = idleTap(rng);
        int perRepeat = 8;

        switch (symbol)
        {
            case 'a':
            {
                type = ActionType::StrafeLeft;
                baseDuration = sideTap(rng);
                perRepeat = 14;
                break;
            }

            case 'd':
            {
                type = ActionType::StrafeRight;
                baseDuration = sideTap(rng);
                perRepeat = 14;
                break;
            }

            case 'w':
            {
                type = ActionType::MoveForward;
                baseDuration = forwardTap(rng);
                perRepeat = 12;
                break;
            }

            case 's':
            {
                type = ActionType::MoveBackward;
                baseDuration = backTap(rng);
                perRepeat = 12;
                break;
            }

            case 'c':
            {
                type = ActionType::Jump;
                baseDuration = jumpTap(rng);
                perRepeat = 0;
                break;
            }

            default:
            {
                type = ActionType::Idle;
                baseDuration = idleTap(rng);
                perRepeat = 6;
                break;
            }
        }

        int durationMs = baseDuration + static_cast<int>(run - 1) * perRepeat;

        if (durationMs > 650)
        {
            durationMs = 650;
        }

        actions.push_back({
            type,
            durationMs
        });

        if (type != ActionType::Idle)
        {
            actions.push_back({
                ActionType::Idle,
                idleTap(rng)
            });
        }

        i += run;
    }

    return actions;
}

void Bot::WorkerLoop()
{
    std::random_device rd;

    std::mt19937 rng(rd());

    while (m_running.load())
    {
        std::vector<Bot::Action> scenario =
            GetRandomScenario(rng);

        for (const Bot::Action& action : scenario)
        {
            if (!m_running.load())
            {
                return;
            }

            ExecuteAction(action);
        }

        for (int i = 0; i < 50 && m_running.load(); ++i)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }
    }
}