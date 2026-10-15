CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2

.PHONY: all test
all: build/ready-queue

build/ready-queue: src/main.cpp include/scheduler.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Iinclude src/main.cpp -o $@

build/scheduler-tests: tests/scheduler_test.cpp include/scheduler.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Iinclude tests/scheduler_test.cpp -o $@

test: all build/scheduler-tests
	./build/scheduler-tests
	python3 tests/cli_test.py ./build/ready-queue
