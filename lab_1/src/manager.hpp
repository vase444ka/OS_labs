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
            PIPE_CREATION_FAILED, PIPE_CONNECTION_FAILED, SUCCESS, PROCESS_CREATION_FAILED
        };

        explicit ManagerBase(int x_arg);
        RunExitCode run();
        std::optional <bool> getResult(){return _res;}

    private:
        static auto _runWorker(const std::string &command_line) -> std::optional<PROCESS_INFORMATION>;
        static auto _getResult(HANDLE pipe) -> std::optional<bool>;

        int _x_arg;
        std::optional<bool> _res;
    };

} //namespace spos::lab1

#endif //LAB_MANAGER_HPP
