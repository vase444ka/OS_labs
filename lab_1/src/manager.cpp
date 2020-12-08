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

        std::wcout << "Waiting for a client to connect to the pipe..." << std::endl;

        // This call blocks until a client process connects to the pipe
        BOOL result = ConnectNamedPipe(pipe, nullptr);
        if (!result) {
            std::wcout << "Failed to make connection on named pipe." << std::endl;
            // look up error code here using GetLastError()
            return std::nullopt;
        }

        std::wcout << "Receiving data from pipe..." << std::endl;

        // This call blocks until a client process reads all the data

        std::any buffer;
        DWORD numBytesRead = 0;
        result = ReadFile(
                pipe,
                &buffer, // the data from the pipe will be put here
                sizeof(std::any), // number of bytes allocated
                &numBytesRead, // this will store number of bytes actually read
                0 // not using overlapped IO
        );
        std::wcout<<"RES--- "<<result<<std::endl;

        if (result) {
            try {
                char* worker_return = std::any_cast<char*>(buffer);
                std::wcout << "Number of bytes read: " << numBytesRead <<":::::" << worker_return<< std::endl;

                return worker_return;
            } catch (const std::exception& e) {
                std::wcout << "sosi bibu "<< std::endl;
                std::wcout << e.what() << std::endl;
            }
        }

        std::wcout << "Failed to read data." << std::endl;
        // look up error code here using GetLastError()
        return std::nullopt;

    }

    Manager::RunExitCode Manager::run() {
        //creating pipes
        std::wcout << "Creating an instance of a named pipe..." << std::endl;

        _f_func_pipe = CreateNamedPipe(
                R"(\\.\pipe\f_func_pipe)", // name of the pipe
                PIPE_ACCESS_INBOUND, // 1-way pipe -- receive only
                PIPE_TYPE_BYTE, // send data as a byte stream
                1, // only allow 1 instance of this pipe
                0, // no outbound buffer
                sizeof(std::any), // no inbound buffer
                0, // use default wait time
                nullptr // use default security attributes
        );

        if (_f_func_pipe == nullptr || _f_func_pipe == INVALID_HANDLE_VALUE) {
            std::wcout << "Failed to create inbound pipe instance.";
            // look up error code here using GetLastError()
            return PIPE_PROCESS_CREATION_FAILED;
        }

        _g_func_pipe = CreateNamedPipe(
                R"(\\.\pipe\g_func_pipe)", // name of the pipe
                PIPE_ACCESS_INBOUND, // 1-way pipe -- receive only
                PIPE_TYPE_BYTE, // send data as a byte stream
                1, // only allow 1 instance of this pipe
                0, // no outbound buffer
                sizeof(std::any), // no inbound buffer
                0, // use default wait time
                nullptr // use default security attributes
        );

        if (_g_func_pipe == nullptr || _g_func_pipe == INVALID_HANDLE_VALUE) {
            std::wcout << "Failed to create inbound pipe instance.";
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
            //std::wcout<<"shit"<<std::endl;
            const auto ready_future_it = std::find_if(
                    func_futures.begin(),
                    func_futures.end(),
                    [](auto &fut) { return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; });


            if (ready_future_it != func_futures.end()) {
                func_results[std::distance(func_futures.begin(), ready_future_it)] = (*ready_future_it).get().value();
                std::cout <<"\n------------------------\nres_no: " << std::distance(func_futures.begin(), ready_future_it) << std::endl;
                func_futures.erase(ready_future_it);
            }
        }

        //waiting till async calls response

        // Close the pipe (automatically disconnects client too)
        CloseHandle(_f_func_pipe);
        CloseHandle(_g_func_pipe);

        return SUCCESS;
    }
}
