#pragma once
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ready_queue {
struct Task {
    std::string id;
    int priority = 0;
    std::vector<std::string> dependencies;
};

// Pure planning: this does not execute commands or create threads.
inline std::vector<std::string> schedule(const std::vector<Task>& tasks) {
    std::unordered_map<std::string, std::size_t> index;
    for (std::size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].id.empty() || !index.emplace(tasks[i].id, i).second)
            throw std::invalid_argument("Task IDs must be nonempty and unique");
    }
    std::vector<std::size_t> remaining(tasks.size(), 0);
    std::vector<std::vector<std::size_t>> dependents(tasks.size());
    for (std::size_t i = 0; i < tasks.size(); ++i) {
        std::unordered_map<std::string, bool> seen;
        for (const auto& dependency : tasks[i].dependencies) {
            const auto found = index.find(dependency);
            if (found == index.end())
                throw std::invalid_argument("Unknown dependency: " + dependency);
            if (!seen.emplace(dependency, true).second)
                throw std::invalid_argument("Duplicate dependency: " + dependency);
            dependents[found->second].push_back(i);
            ++remaining[i];
        }
    }
    const auto lower_priority = [&tasks](std::size_t a, std::size_t b) {
        if (tasks[a].priority != tasks[b].priority)
            return tasks[a].priority < tasks[b].priority;
        return tasks[a].id > tasks[b].id;
    };
    std::priority_queue<std::size_t, std::vector<std::size_t>, decltype(lower_priority)>
        ready(lower_priority);
    for (std::size_t i = 0; i < tasks.size(); ++i)
        if (remaining[i] == 0) ready.push(i);
    std::vector<std::string> order;
    while (!ready.empty()) {
        const auto next = ready.top();
        ready.pop();
        order.push_back(tasks[next].id);
        for (const auto dependent : dependents[next])
            if (--remaining[dependent] == 0) ready.push(dependent);
    }
    if (order.size() != tasks.size())
        throw std::invalid_argument("Dependency cycle detected");
    return order;
}
} // namespace ready_queue
