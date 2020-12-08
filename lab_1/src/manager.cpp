#include "manager.hpp"

namespace spos::lab1 {

Manager::Manager(std::string op_name, int x_arg) :
        _x_arg{x_arg}, _op_name{std::move(op_name)} {}

std::optional<PROCESS_INFORMATION> Manager::_runWorker(const std::string &command_line) {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&process_info, sizeof(process_info));

    if (!CreateProcess("f_g_funcs.exe", (char *) command_line.c_str(),
                       nullptr, nullptr, false, 0, nullptr, nullptr,
                       &startup_info, &process_info)
            ) {
        return std::nullopt;
    }

    return process_info;
}

static auto _getFunctionResult(HANDLE pipe) -> std::optional<std::string> {

    std::cout << "Waiting for a client to connect to the pipe..." << std::endl;

    // This call blocks until a client process connects to the pipe
    BOOL result = ConnectNamedPipe(pipe, nullptr);
    if (!result) {
        std::cout << "Failed to make connection on named pipe." << std::endl;
        // look up error code here using GetLastError()
        return std::nullopt;
    }

    std::cout << "Receiving data from pipe..." << std::endl;

    // This call blocks until a client process reads all the data

    char *buffer = new char[sizeof(int)];
    DWORD numBytesRead = 0;
    result = ReadFile(
            pipe,
            buffer, // the data from the pipe will be put here
            sizeof(int), // number of bytes allocated
            &numBytesRead, // this will store number of bytes actually read
            0 // not using overlapped IO
    );

    if (result) {
            std::cout << "Number of bytes read: " << numBytesRead <<std::endl;
            buffer[4] = '\0';
            std::cout << "Value read "<< buffer << std::endl;

            return std::string(buffer);
    }

    std::cout << "Failed to read data." << std::endl;
    // look up error code here using GetLastError()
    return std::nullopt;

}

Manager::RunExitCode Manager::run() {
    //creating pipes
    std::cout << "Creating an instance of a named pipe..." << std::endl;

    _f_func_pipe = CreateNamedPipe(
            R"(\\.\pipe\f_func_pipe)", // name of the pipe
            PIPE_ACCESS_INBOUND, // 1-way pipe -- receive only
            PIPE_TYPE_BYTE, // send data as a byte stream
            1, // only allow 1 instance of this pipe
            0, // no outbound buffer
            0, // no inbound buffer
            0, // use default wait time
            nullptr // use default security attributes
    );

    if (_f_func_pipe == nullptr || _f_func_pipe == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create inbound pipe instance.";
        // look up error code here using GetLastError()
        return PIPE_PROCESS_CREATION_FAILED;
    }

    _g_func_pipe = CreateNamedPipe(
            R"(\\.\pipe\g_func_pipe)", // name of the pipe
            PIPE_ACCESS_INBOUND, // 1-way pipe -- receive only
            PIPE_TYPE_BYTE, // send data as a byte stream
            1, // only allow 1 instance of this pipe
            0, // no outbound buffer
            0, // no inbound buffer
            0, // use default wait time
            nullptr // use default security attributes
    );

    if (_g_func_pipe == nullptr || _g_func_pipe == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create inbound pipe instance.";
        // look up error code here using GetLastError()
        return PIPE_PROCESS_CREATION_FAILED;
    }

    std::vector<std::future<std::optional<std::string>>> func_futures;
    func_futures.emplace_back(std::async(std::launch::async, _getFunctionResult, _f_func_pipe));
    //func_futures.emplace_back(std::async(std::launch::async, _getFunctionResult, _g_func_pipe));


    //setting up workers for f and g
    _f_process_info = _runWorker(" " + _op_name + " f " + std::to_string(_x_arg) + " " + R"(\\.\pipe\f_func_pipe)");
    //_g_process_info = _runWorker(" " + _op_name + " g " + std::to_string(_x_arg) + " " + R"(\\.\pipe\g_func_pipe)");

    if (!_f_process_info.has_value() /*|| !_g_process_info.has_value()*/) {
        CloseHandle(_f_func_pipe);
        CloseHandle(_g_func_pipe);
        return PROCESS_CREATION_FAILED;
    }

    std::vector<std::optional<std::string>> func_results(func_futures.size(), std::nullopt);
    while (!func_futures.empty()) {
        //std::cout<<"shit"<<std::endl;
        const auto ready_future_it = std::find_if(
                func_futures.begin(),
                func_futures.end(),
                [](auto &fut) { return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; });


        if (ready_future_it != func_futures.end()) {
            if ((*ready_future_it).get())
                func_results[std::distance(func_futures.begin(), ready_future_it)] = (*ready_future_it).get().value();
            std::cout << "\n------------------------\nres_no: "
                      << std::distance(func_futures.begin(), ready_future_it) << std::endl;
            func_futures.erase(ready_future_it);
        }
    }

    // Close the pipe (automatically disconnects client too)
    CloseHandle(_f_func_pipe);
    CloseHandle(_g_func_pipe);

    return SUCCESS;
}
}//namespace spos::lab1
