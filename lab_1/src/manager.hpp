#ifndef LAB_MANAGER_HPP
#define LAB_MANAGER_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <string>
#include <optional>
#include <vector>
#include <algorithm>
#include <future>
#include <iostream>
#include <fstream>

namespace spos::lab1 {

    class Manager {
    public:
        enum RunExitCode {
            SETUP_FAILED, PIPE_CONNECTION_FAILED, SUCCESS, SHORT_CIRCUIT_EVALUATED, CANCELLED
        };
        std::ofstream out;

        explicit Manager(int x_arg);
        void run();
        void printResult();

    private:
        static auto _runWorker(const std::string &command_line) -> std::optional<PROCESS_INFORMATION>;
        auto _getResult(HANDLE pipe) -> std::optional<bool>;
        bool _setup(std::string);
        RunExitCode _compute();

        int _x_arg;
        std::optional<bool> _res;
        RunExitCode _run_msg;

        std::vector<std::future<std::optional<bool>>> _func_futures;
        std::vector<PROCESS_INFORMATION> _process_info;
        std::vector<HANDLE> _pipe;
        std::vector<bool> _func_results;
        std::vector<bool> _status;
    };

} //namespace spos::lab1

#endif //LAB_MANAGER_HPP
