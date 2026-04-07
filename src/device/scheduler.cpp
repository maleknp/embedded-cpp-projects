#include "device/scheduler.hpp"
#include "utils/logger.hpp"

namespace etc {

void Scheduler::addTask(const std::string& name, TaskFunc callback, uint32_t period_ms) {
    Task t;
    t.name = name;
    t.callback = std::move(callback);
    t.period_ms = period_ms;
    t.last_run = std::chrono::steady_clock::now();

    tasks_.push_back(std::move(t));
    Logger::instance().info("SCHED", "Registered task '" + name + "' @ " + std::to_string(period_ms) + " ms");
}

void Scheduler::tick() {
    auto now = std::chrono::steady_clock::now();

    for (auto& task : tasks_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - task.last_run).count();
        if (elapsed >= task.period_ms) {
            Logger::instance().debug("SCHED", "Running task '" + task.name + "'");
            task.callback();
            task.last_run = now;
        }
    }
}

} // namespace etc
