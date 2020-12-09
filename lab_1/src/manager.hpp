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
#include <any>

namespace spos::lab1 {

    class Manager {
    public:
        enum RunExitCode {
            PIPE_PROCESS_CREATION_FAILED, PIPE_CONNECTION_FAILED, SUCCESS, PROCESS_CREATION_FAILED
        };

        Manager(std::string op_name, int x_arg);
        RunExitCode run();
        std::optional <bool> getResult(){return _res;}

    private:
        static auto _runWorker(const std::string &command_line) -> std::optional<PROCESS_INFORMATION>;
        static auto _getFunctionResult(HANDLE pipe) -> std::optional<bool>;

        int _x_arg;
        std::optional<bool> _res;
        std::string _op_name;
        std::optional<PROCESS_INFORMATION> _f_process_info;
        std::optional<PROCESS_INFORMATION> _g_process_info;
        HANDLE _f_func_pipe, _g_func_pipe;
    };

} //namespace spos::lab1

#endif //LAB_MANAGER_HPP
