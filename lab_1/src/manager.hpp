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

namespace spos::lab1 {

    class Manager {
    public:
        enum RunExitCode {
            SUCCESS, WSA_STARTUP_FAILED, SOCKET_CONNECTION_ERROR, PROCESS_CREATION_FAILED
        };

        Manager(std::string op_name, int x_arg);
        RunExitCode run();

    private:
        static auto _runWorker(const std::string &command_line) -> std::optional<PROCESS_INFORMATION>;
        //_getFunctionResult implementation

        int _x_arg;
        std::string _op_name;
        std::optional<PROCESS_INFORMATION> _f_process_info;
        std::optional<PROCESS_INFORMATION> _g_process_info;
    };

} //namespace spos::lab1

#endif //LAB_MANAGER_HPP
