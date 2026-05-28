#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <string_view>
#include <thread>

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
    static int KeyFor(char symbol);
    void HoldKey(int key, int durationMs);
    void ReleaseKey(int key);
    void ReleaseAllKeys();
    void RunScenario(std::string_view pattern);
    void WorkerLoop();

    std::atomic<bool> m_running;
    std::atomic<int> m_actions;
    std::thread m_thread;
    mutable std::mutex m_stateMutex;
    std::chrono::steady_clock::time_point m_startTime;
    float m_lastSessionSeconds;
};
