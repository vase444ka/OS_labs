#include "manager.hpp"

namespace spos::lab1 {
    Manager::Manager(std::string op_name, int x_arg) :
            _x_arg{x_arg}, _op_name{std::move(op_name)} {}

    std::optional<PROCESS_INFORMATION> Manager::_runWorker(const std::string &command_line) {

    }

    static auto _getFunctionResult(SOCKET listen_socket) -> std::optional<std::string>{

    }

    Manager::RunExitCode Manager::run() {

    }
}
