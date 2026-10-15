# Ready Queue

A C++17 command-line planner that orders tasks by dependency and priority.
Inspired by build pipelines: tests must wait for compilation, but independent
tasks can be selected according to their priority.

## Build and run

Requires CMake 3.16+ and a C++17 compiler. No external libraries.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/ready-queue examples/build.tasks
```

Expected output:

```text
lint
compile
test
package
```

Without CMake, use `make` and `make test` (the CLI tests require Python 3).
The Makefile uses only the system C++ compiler and Python standard library.

## Input format

Each nonblank line is `id priority [dependency ...]`. IDs are unique,
case-sensitive tokens. Higher integer priorities run first among ready tasks;
ties use ascending task IDs. Priorities may be negative. `#` starts a comment.
Dependencies may be declared before or after the task that references them.

Unknown dependencies, repeated IDs or dependencies, empty IDs, malformed
priorities, and cycles are rejected. The CLI emits no plan until the entire
graph is validated. Exit codes: 0 success, 1 invalid data, 2 usage or open error.

## Algorithm and C++ concepts

The planner uses Kahn's topological sort with a priority queue of ready nodes.
An adjacency list tracks dependents; an indegree counter tracks unmet
dependencies. Completing a node decrements its dependents' counters and releases
newly ready nodes. Unscheduled nodes after the queue empties indicate a cycle.

Expected time is O(V log V + E), assuming average constant-time hash lookup,
with string comparison and hashing costs omitted. Space is O(V + E).
The iterative algorithm avoids recursion depth limits on long chains.

This project demonstrates STL containers, custom comparators, const references,
value ownership, exception handling, RAII file streams, and automated testing.
It produces a deterministic serial plan; it does not execute jobs, create worker
threads, persist state, or claim production throughput benchmarks.

## Tests

Tests cover priorities, deterministic ties, dependency constraints, a diamond
graph, unknown references, duplicate IDs and dependencies, self-loops, cycles,
empty input, and a 10,000-node chain. CTest also checks the example CLI output.
Test assertions remain active in release builds. GitHub Actions is configured
to build and test on Linux and macOS.

## Learning roadmap

1. Trace the diamond-graph test by hand and explain each indegree update.
2. Add a command that displays execution layers for unlimited parallel workers.
3. Add cycle diagnostics that identify participating task IDs.
4. Measure generated graph workloads before considering performance changes.

## Provenance

Created with AI assistance in October 2026 as a C++ learning project. The Git
history uses reconstructed dates distributed across 2026 at the owner's request.
Those dates do not represent when development took place.
