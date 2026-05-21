#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class Bot
{
public:
    Bot();

    ~Bot();

    void Start();

    void Stop();

    bool IsRunning() const;

    int GetActions() const;

    float GetSessionTime() const;

private:
    enum class ActionType
    {
        MoveForward,
        MoveBackward,
        StrafeLeft,
        StrafeRight,
        Jump,
        Idle
    };

    struct Action
    {
        ActionType type;
        int durationMs;
    };

    void PressKey(int key, int durationMs);
    void ExecuteAction(const Action& action);
    std::vector<Action> GetRandomScenario(std::mt19937& rng);
    void WorkerLoop();

private:
    std::atomic<bool> m_running;
    std::atomic<int> m_actions;
    std::thread m_thread;
    mutable std::mutex m_stateMutex;

    std::chrono::steady_clock::time_point m_startTime;
    float m_lastSessionSeconds;
};