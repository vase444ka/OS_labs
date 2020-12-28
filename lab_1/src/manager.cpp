#include "manager.hpp"

namespace spos::lab1 {

Manager::Manager(int x_arg) :
        _x_arg{x_arg} {
    out.open("output.txt");
}

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

auto Manager::_getResult(HANDLE pipe) -> std::optional<bool> {
    out << "Waiting for a client to connect to the _pipe... Thread no" << std::this_thread::get_id()<< std::endl;
    BOOL result = ConnectNamedPipe(pipe, nullptr);

    if (!result){
        out << "Failed to connect _pipe." << std::endl;
        CloseHandle(pipe);
        return std::nullopt;
    }

    out << "Receiving data from _pipe..." << std::endl;
    char *buffer = new char[sizeof(bool)];
    DWORD numBytesRead = 0;
    result = ReadFile(
            pipe,
            buffer,
            sizeof(bool),
            &numBytesRead,
            nullptr
    );

    if (result) {
        bool res = (bool)(*buffer);
        delete [] buffer;
        out << "Value read "<< res <<"_____size(bytes)________"<<numBytesRead<< std::endl;
        CloseHandle(pipe);
        return res;
    }

    out << "Failed to read data." << std::endl;
    CloseHandle(pipe);
    return std::nullopt;
}

bool Manager::_setup(std::string func_name) {
    out << "Creating an instance of a named _pipe..." << std::endl;
    std::string pipe_name = R"(\\.\pipe\func_pipe_)";
    pipe_name += func_name;

    _pipe.emplace_back(CreateNamedPipeA(
            pipe_name.c_str(), // name of the _pipe
            PIPE_ACCESS_INBOUND, // 1-way _pipe -- receive only
            PIPE_TYPE_BYTE, // send data as a byte stream
            1, // only allow 1 instance of this _pipe
            0, // no outbound buffer
            0, // no inbound buffer
            0, // use default wait time
            nullptr // use default security attributes
    ));

    if (_pipe.back() == nullptr || _pipe.back() == INVALID_HANDLE_VALUE) {
        out << "Failed to create inbound pipe instance." << std::endl;
        return false;
    }

    _func_futures.emplace_back(std::async(std::launch::async, &Manager::_getResult, this, _pipe.back()));
    auto tmp_process_info = _runWorker(" " + func_name + " " + std::to_string(_x_arg) + " " + pipe_name);
    if (tmp_process_info.has_value()) {
        _process_info.push_back(tmp_process_info.value());
    }
    else {
        for (auto &it : _pipe)
            CloseHandle(it);
        return false;
    }

    return true;
}

Manager::RunExitCode Manager::_compute() {
    _func_results.resize(2);
    _status.resize(2, false);
    int completed_count = 0;
    while (completed_count < 2) {
        if (GetAsyncKeyState(VK_ESCAPE)){
            UINT exit_code;
            for (int i = 0; i < 2; i++)
                if (!_status[i]) {
                    TerminateProcess(_process_info[i].hProcess, exit_code);
                    CloseHandle(_pipe[i]);
                }
            return CANCELLED;
        }

        const auto ready_future_it = std::find_if(
                _func_futures.begin(),
                _func_futures.end(),
                [](auto &fut) { return fut.valid() && fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; });


        if (ready_future_it != _func_futures.end()) {
            std::optional <bool> ready_future = (*ready_future_it).get();
            if (!ready_future)
                return PIPE_CONNECTION_FAILED;

            int res_no = std::distance(_func_futures.begin(), ready_future_it);
            _func_results[res_no] = ready_future.value();
            _status[res_no] = true;
            if (_func_results[res_no] && completed_count < 2){
                UINT exit_code;
                TerminateProcess(_process_info[(res_no + 1) % 2].hProcess, exit_code);
                _res = true;
                return SHORT_CIRCUIT_EVALUATED;
            }
            out << "\n------------------------\nres_no: "<< res_no <<" = "<<ready_future.value()<< std::endl;
            completed_count++;
        }
    }

    _res = (_func_results[0] || _func_results[1]);
    return SUCCESS;
}

void Manager::run() {
    if (!_setup("f") || !_setup("g")) {
        _run_msg = SETUP_FAILED;
        return;
    }
    _run_msg =  _compute();
}

void Manager::printResult() {
    std::cout<<"----COMPUTATION STATUS---- x = "<<_x_arg<<" ----"<<std::endl;
    switch (_run_msg) {
        case SETUP_FAILED:
            std::cout<<"SETUP FAILED"<<std::endl;
            break;
        case PIPE_CONNECTION_FAILED:
            for (auto it: _status)
                if (it)
                    std::cout<<"> one function computed"<<std::endl;
            std::cout<<"> (another) function(s) pipe(s) did not connect properly"<<std::endl;
            break;
        case SUCCESS:
            std::cout<<"> f resulted into "<<_func_results[0]<<std::endl;
            std::cout<<"> g resulted into "<<_func_results[1]<<std::endl;
            std::cout<<"> binary operation resulted into"<<_res.value()<<std::endl;
            break;
        case SHORT_CIRCUIT_EVALUATED:
            for (auto it: _status)
                if (it)
                    std::cout<<"> one function returned 1"<<std::endl;
            std::cout<<"> result is short circuit evaluated"<<std::endl;
            break;
        case CANCELLED:
            int i = 0;
            while(!_status[i] && i < 2) i++;
            if (i < 2)
                std::cout<<"> function no."<<i<<" is computed. Function no."<<(i + 1) % 2<<" computation is cancelled"<<std::endl;
            std::cout<<"> result is not defined"<<std::endl;
            break;
    }

}


}//namespace spos::lab1
