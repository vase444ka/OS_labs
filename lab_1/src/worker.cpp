#include "demofuncs.hpp"

#include <iostream>
#include <windows.h>
#include <map>
#include <functional>
#include <any>

namespace spos::lab1::utils {

class Worker {
    char* _x;

public:
    explicit Worker(char* x)
            : _x(x) {
    }

    int runWorker(char *pipe_name, char *func_id, char *op_name) {
        std::cout << "(Worker) Connecting to the pipe..." << std::endl;

        // Open the named pipe
        HANDLE pipe = CreateFile(
                pipe_name,
                GENERIC_WRITE,
                FILE_SHARE_WRITE,
                nullptr,
                CREATE_NEW,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
        );

        if (pipe == INVALID_HANDLE_VALUE) {
            std::cout << "(Worker) Failed to connect to pipe" << std::endl;
            return 1;
        }

        auto func_res = spos::lab1::demo::f_func <spos::lab1::demo::OR> (2);

        char* tmp_buffer = (char*)&func_res;//cast func result into char*
        char* buffer = new char[sizeof(func_res)];
        for (int i = 0; i<sizeof(func_res); i++){
            buffer[i] = tmp_buffer[i];
        }

        std::cout << "(Worker) Writing data to pipe..." << std::endl;

        DWORD numBytesWritten = 0;
        BOOL isWritten = WriteFile(
                pipe,
                buffer,
                sizeof(func_res),
                &numBytesWritten,
                nullptr
        );

        if (isWritten) {
            std::cout << "(Worker) Value was written: " << buffer << "**"<<numBytesWritten<<std::endl;
        } else {
            std::cout << "(Worker) Failed to write the data to a pipe." << std::endl;
        }

        CloseHandle(pipe);
        delete[] buffer;
        std::cout << "(Worker) Done" << std::endl;

        return 0;
    }

};
}//namespace spos::lab1::utils

/*
argv:
    [1] operation_name,
    [2] function_id,
    [3] x_argument,
    [4] pipe_name
*/

int main(int argc, char *argv[]) {
    spos::lab1::utils::Worker worker = spos::lab1::utils::Worker(argv[3]);

    return worker.runWorker(argv[4], argv[2], argv[1]);
}
