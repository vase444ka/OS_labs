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
#include <atomic>

namespace spos::lab1 {

    class ManagerBase {
    public:
        enum RunExitCode {
            SETUP_FAILED, PIPE_CONNECTION_FAILED, SUCCESS, SHORT_CIRCUIT_EVALUATED
        };

        explicit ManagerBase(int x_arg);
        RunExitCode run();
        std::optional <bool> getResult(){return _res;}

    private:
        static auto _runWorker(const std::string &command_line) -> std::optional<PROCESS_INFORMATION>;
        static auto _getResult(HANDLE pipe) -> std::optional<bool>;
        bool _setup(std::string);
        bool _compute();

        int _x_arg;
        std::optional<bool> _res;

        std::vector<std::future<std::optional<bool>>> _func_futures;
        std::vector<PROCESS_INFORMATION> _process_info;
        std::vector<HANDLE> _pipe;
    };

} //namespace spos::lab1

#endif //LAB_MANAGER_HPP
