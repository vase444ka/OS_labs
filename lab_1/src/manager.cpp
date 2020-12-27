#include "manager.hpp"

#include <thread>//debug

namespace spos::lab1 {

ManagerBase::ManagerBase(int x_arg) :
        _x_arg{x_arg} {}

std::optional<PROCESS_INFORMATION> ManagerBase::_runWorker(const std::string &command_line) {
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

auto ManagerBase::_getResult(HANDLE pipe) -> std::optional<bool> {
    std::cout << "Waiting for a client to connect to the pipe... Thread no" << std::this_thread::get_id()<< std::endl;

    //debug
    LPOVERLAPPED lpOverlapped;
    lpOverlapped->hEvent = CreateEvent(
            NULL,               // default security attributes
            TRUE,               // manual-reset event
            TRUE,              // initial state is nonsignaled
            NULL  // object name
    );

    std::cout<<"Connecting the pipe..."<<std::endl;
    // This call blocks until a client process connects to the pipe TODO overlapping
    BOOL result = ConnectNamedPipe(pipe, lpOverlapped);
    if (!result && GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_PIPE_CONNECTED) {
        std::cout << "Failed to make connection on named pipe." << std::endl;
        CloseHandle(pipe);
        return std::nullopt;
    }

    if (!result && GetLastError() == ERROR_IO_PENDING){
        WaitForSingleObject(lpOverlapped->hEvent, INFINITE);
    }

    std::cout << "Receiving data from pipe..." << std::endl;

    // This call blocks until a client process sends all the data
    char *buffer = new char[sizeof(bool)];
    DWORD numBytesRead = 0;
    result = ReadFile(
            pipe,
            buffer, // the data from the pipe will be put here
            sizeof(bool), // number of bytes allocated
            &numBytesRead, // this will store number of bytes actually read
            nullptr // not using overlapped IO
    );

    if (result) {
        bool res = (bool)(*buffer);
        delete [] buffer;
        std::cout << "Value read "<< res <<"_____size(bytes)________"<<numBytesRead<< std::endl;

        CloseHandle(pipe);
        return res;
    }

    std::cout << "Failed to read data." << std::endl;
    CloseHandle(pipe);
    return std::nullopt;

}



ManagerBase::RunExitCode ManagerBase::run() {
    std::vector<std::future<std::optional<bool>>> func_futures;
    std::vector<PROCESS_INFORMATION> process_info;
    std::vector<HANDLE> pipe;

    for (int i = 0; i < 2; i++) {
        std::cout << "Creating an instance of a named pipe..." << std::endl;
        std::string func_name = i == 0 ? "f" : "g";
        std::string pipe_name = R"(\\.\pipe\func_pipe_)";
        pipe_name += func_name;

        //pipe creation and connection
        pipe.emplace_back(CreateNamedPipeA(
                pipe_name.c_str(), // name of the pipe
                PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, // 1-way pipe -- receive only
                PIPE_TYPE_BYTE, // send data as a byte stream TODO decide whether we need overlapped flag
                1, // only allow 1 instance of this pipe
                0, // no outbound buffer
                0, // no inbound buffer
                0, // use default wait time
                nullptr // use default security attributes
        ));

        if (pipe[i] == nullptr || pipe[i] == INVALID_HANDLE_VALUE) {
            std::cout << "Failed to create inbound pipe instance." << std::endl;
            std::cout << GetLastError() << std::endl;
            return PIPE_CREATION_FAILED;
        }

        //process creation
        func_futures.emplace_back(std::async(std::launch::async, _getResult, pipe[i]));
        auto tmp_process_info = _runWorker(" " + func_name + " " + std::to_string(_x_arg) + " " + pipe_name);
        if (tmp_process_info.has_value()) {
            process_info.push_back(tmp_process_info.value());
        }
        else {
            for (auto &it : pipe)
                CloseHandle(it);
            return PROCESS_CREATION_FAILED;
        }
    }

    //demo (func_futures needed + some process info)

    std::vector<bool> func_results;
    int tmp_clock = 0;
    while (!func_futures.empty()) {
        tmp_clock++;//debug
        if (tmp_clock % (int)1e3 == 0) std::cout<<tmp_clock<<std::endl;
        if (tmp_clock > 1e6){//debug
            UINT exit_code;
            TerminateProcess(process_info[1].hProcess, exit_code);
            std::cout<<"closing g pipe------------------1"<<std::endl;
            break;//debug
        }//debug

        const auto ready_future_it = std::find_if(
                func_futures.begin(),
                func_futures.end(),
                [](auto &fut) { return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; });


        if (ready_future_it != func_futures.end()) {
            std::optional <bool> ready_future = (*ready_future_it).get();
            if (!ready_future)
                return PIPE_CONNECTION_FAILED;

            int res_no = std::distance(func_futures.begin(), ready_future_it);
            func_results.push_back(ready_future.value());
            std::cout << "\n------------------------\nres_no: "<< res_no <<" = "<<ready_future.value()<< std::endl;
            func_futures.erase(ready_future_it);
        }
    }
    if (func_results.size() == 2)
        _res = (func_results[0] || func_results[1]);

    std::cout<<"----manager run returns----"<<std::endl;
    return SUCCESS;
}
}//namespace spos::lab1
