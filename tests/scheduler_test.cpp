#include "scheduler.hpp"
#include <iostream>
#include <functional>

void check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
void rejects(const std::vector<ready_queue::Task>& tasks) {
    try { ready_queue::schedule(tasks); }
    catch (const std::invalid_argument&) { return; }
    throw std::runtime_error("Expected invalid graph to be rejected");
}
int main() {
    using ready_queue::schedule;
    try {
        check(schedule({}).empty(), "Empty graph");
        check(schedule({{"b", 1, {}}, {"a", 1, {}}}) == std::vector<std::string>({"a", "b"}), "Stable lexical ties");
        check(schedule({{"low", -1, {}}, {"high", 100, {}}}) == std::vector<std::string>({"high", "low"}), "Priority ordering");
        check(schedule({{"compile", 0, {}}, {"test", 100, {"compile"}}, {"lint", 1, {}}}) == std::vector<std::string>({"lint", "compile", "test"}), "Dependencies override priority");
        check(schedule({{"a", 0, {}}, {"b", 0, {"a"}}, {"c", 0, {"a"}}, {"d", 9, {"b", "c"}}}) == std::vector<std::string>({"a", "b", "c", "d"}), "Diamond graph");
        rejects({{"a", 0, {"missing"}}});
        rejects({{"a", 0, {}}, {"a", 0, {}}});
        rejects({{"", 0, {}}});
        rejects({{"a", 0, {"a"}}});
        rejects({{"a", 0, {"b"}}, {"b", 0, {"a"}}});
        rejects({{"a", 0, {}}, {"b", 0, {"a", "a"}}});
        std::vector<ready_queue::Task> chain;
        for (int i = 0; i < 10000; ++i)
            chain.push_back({std::to_string(i), i, i ? std::vector<std::string>{std::to_string(i - 1)} : std::vector<std::string>{}});
        const auto order = schedule(chain);
        check(order.size() == 10000 && order.front() == "0" && order.back() == "9999", "Large chain without recursion");
        std::cout << "12 scheduler cases passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
