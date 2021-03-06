// =============================================================================
//                        !!! DO NOT EDIT THIS FILE !!!
// =============================================================================

#include "common.h"
#include "scheduler.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

using VS = std::vector<std::string>;

template <typename T>
static bool vcmp(std::vector<T> v1, std::vector<T> v2)
{
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    return v1 == v2;
}

static void print_procs(const std::vector<Process> & procs, int indent = 0)
{
    std::string inds(indent, ' ');
    std::cout << inds
              << "+---------------------------+----------------------+----------------------+------"
                 "----------------+\n"
              << inds
              << "| Id |              Arrival |                Burst |                Start |      "
                 "         Finish |\n"
              << inds
              << "+---------------------------+----------------------+----------------------+------"
                 "----------------+\n";
    for (const auto & p : procs) {
        std::cout << inds << "| " << std::setw(2) << std::right << p.id << " | " << std::setw(20)
                  << p.arrival_time << " | " << std::setw(20) << p.burst << " | " << std::setw(20)
                  << p.start_time << " | " << std::setw(20) << p.finish_time << " |"
                  << "\n";
    }
    std::cout << inds
              << "+---------------------------+----------------------+----------------------+------"
                 "----------------+\n";
}

static void test(
    const std::string & test_name,
    int64_t q,
    int64_t maxs,
    const std::vector<Process> & procs,
    const std::vector<int> & seq)
{
    std::cout << "- running " << test_name << " ..." << std::flush;

    Timer timer;
    std::vector<int> ret_seq;
    auto ret_procs = procs;
    simulate_rr(q, maxs, ret_procs, ret_seq);
    std::cout << "  - finished in " << std::fixed << std::setprecision(4) << timer.elapsed() << "s";
    bool ok = true;
    if (! vcmp(seq, ret_seq)) ok = false;
    ok &= procs.size() == ret_procs.size();
    if (ok) {
        for (size_t i = 0; i < procs.size(); i++) {
            ok &= procs[i].id == ret_procs[i].id;
            ok &= procs[i].arrival_time == ret_procs[i].arrival_time;
            ok &= procs[i].burst == ret_procs[i].burst;
            ok &= procs[i].start_time == ret_procs[i].start_time;
            ok &= procs[i].finish_time == ret_procs[i].finish_time;
        }
    }
    if (ok) {
        std::cout << Colors::green << " PASSED" << Colors::reset << "\n";
        return;
    }
    std::cout << Colors::bred << " FAILED" << Colors::reset << "\n";
    std::cout << "  - computed sequence: [";
    for (auto p : ret_seq) std::cout << p << ",";
    if (! vcmp(seq, ret_seq)) {
        std::cout << "] " << Colors::bred << "*** WRONG ***" << Colors::reset << "\n"
                  << "  - expected sequence: [";
        for (auto p : seq) std::cout << p << ",";
        std::cout << "]\n";
    } else {
        std::cout << "] ok\n";
    }
    if (procs.size() != ret_procs.size()) {
        std::cout << "  - Ddetected modified processes.size(" << ret_procs.size() << ")\n";
    }
    std::cout << "  - expected processes:\n";
    print_procs(ret_procs, 4);
    std::cout << "  - expected processes:\n";
    print_procs(procs, 4);
    std::cout << "\n";
}

static int run_tests()
{
    std::cout << "Running built-in mini-tests\n";
    test("empty test", 1, 10, {}, {});

    test("one job test", 5, 10, { { 0, 0, 10, 0, 10 } }, { 0 });

    test(
        "lecture slides example",
        3,
        20,
        { { 0, 0, 6, 0, 15 },
          { 1, 0, 6, 3, 20 },
          { 2, 1, 3, 6, 9 },
          { 3, 2, 8, 9, 25 },
          { 4, 3, 2, 15, 17 } },
        { 0, 1, 2, 3, 0, 4, 1, 3 });
    test(
        "simple test",
        1,
        10,
        { { 0, 1, 10, 1, 19 }, { 1, 3, 5, 4, 16 }, { 2, 5, 3, 7, 14 } },
        { -1, 0, 1, 0, 1, 2, 0, 1, 2, 0 });
    return 0;
}

static int run_sched(int64_t quantum, int64_t max_seq_len)
{
    std::cout << "Reading in lines from stdin...\n";

    // read in the process information from stdin
    int line_no = 0;
    std::vector<Process> processes;
    while (1) {
        // read in the next line and quit loop on EOF
        auto line = stdin_readline();
        if (line.size() == 0) break;
        line_no++;
        auto toks = split(line);
        if (toks.size() == 0) continue;
        try {
            if (toks.size() != 2) throw fatal_error() << "need 2 ints per line";
            Process p;
            p.id = processes.size();
            p.arrival_time = std::stoll(toks[0]);
            p.burst = std::stoll(toks[1]);
            processes.push_back(p);
        } catch (std::exception & e) {
            std::cout << "Error on line " << line_no << ": " << e.what() << "\n";
            exit(-1);
        }
    }

    std::cout << "Running simulate_rr(q=" << quantum << ",maxs=" << max_seq_len << ",procs=["
              << processes.size() << "])\n";
    std::vector<int> seq { -2, 1000000, 5000 };
    Timer timer;
    simulate_rr(quantum, max_seq_len, processes, seq);
    std::cout << "Elapsed time  : " << std::fixed << std::setprecision(4) << timer.elapsed()
              << "s\n\n";
    std::cout << "seq = [";
    for (auto p : seq) std::cout << p << ",";
    std::cout << "]\n";
    print_procs(processes);

    return 0;
}
static int usage(const std::string & pname)
{
    std::cout << "Usage:\n"
              << "    " << pname << " test\n"
              << "        - to run few built-in tests\n"
              << "    " << pname << " quantum max_seq_len < input\n"
              << "        - to process input from stdin\n";
    return -1;
}

static int cppmain(const VS & args)
{
    // parse arguments
    if (args.size() == 2 && args[1] == "test") return run_tests();
    else if (args.size() != 3)
        return usage(args[0]);

    try {
        int64_t quantum = std::stoll(args[1]);
        int64_t max_seq_len = std::stoll(args[2]);
        return run_sched(quantum, max_seq_len);
    } catch (...) {
        std::cout << "Could not parse command line arguments.\n";
        return usage(args[0]);
    }
}

int main(int argc, char ** argv)
{
    try {
        return cppmain({ argv + 0, argv + argc });
    } catch (...) {
        std::cerr << "Unhandled exception occurred."
                  << "\n";
        return -1;
    }
}
