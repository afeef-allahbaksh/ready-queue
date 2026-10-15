#include "scheduler.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ready-queue <task-file>\nFormat: id priority [dependency ...]\n";
        return 2;
    }
    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Cannot open task file\n";
        return 2;
    }
    try {
        std::vector<ready_queue::Task> tasks;
        std::string line;
        std::size_t line_number = 0;
        while (std::getline(input, line)) {
            ++line_number;
            line = line.substr(0, line.find('#'));
            std::istringstream fields(line);
            ready_queue::Task task;
            if (!(fields >> task.id)) continue;
            std::string priority;
            if (!(fields >> priority)) throw std::invalid_argument("Missing priority on line " + std::to_string(line_number));
            std::size_t consumed = 0;
            task.priority = std::stoi(priority, &consumed);
            if (consumed != priority.size()) throw std::invalid_argument("Invalid priority on line " + std::to_string(line_number));
            std::string dependency;
            while (fields >> dependency) task.dependencies.push_back(dependency);
            tasks.push_back(std::move(task));
        }
        if (input.bad()) throw std::runtime_error("Could not read task file");
        const auto order = ready_queue::schedule(tasks);
        for (const auto& id : order) std::cout << id << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
