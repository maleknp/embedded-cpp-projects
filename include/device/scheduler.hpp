#pragma once

#include <functional>
#include <vector>
#include <string>
#include <chrono>

namespace etc {

/// A cooperative (non-preemptive) scheduler that runs registered tasks
/// at their configured intervals — typical embedded design pattern.
class Scheduler {
public:
    using TaskFunc = std::function<void()>;

    struct Task {
        std::string name;
        TaskFunc callback;
        uint32_t period_ms;
        std::chrono::steady_clock::time_point last_run;
    };

    /// Register a periodic task.
    void addTask(const std::string& name, TaskFunc callback, uint32_t period_ms);

    /// Call this in the main loop. Runs any tasks whose period has elapsed.
    void tick();

    /// Get the number of registered tasks.
    size_t taskCount() const { return tasks_.size(); }

private:
    std::vector<Task> tasks_;
};

} // namespace etc
