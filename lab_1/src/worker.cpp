#include "demofuncs.hpp"

#include <iostream>
#include <windows.h>
#include <map>
#include <functional>
#include <any>

namespace spos::lab1::utils {
    std::any f_OR(std::any x) {
        return x;
    }

    std::any g_OR(std::any x) {
        return x;
    }

    class Worker {
        std::any _x;
        std::map<char*, std::map<char*, std::function<std::any(std::any)>>> func_op;

    public:
        explicit Worker(std::any x)
        : _x(x){
            func_op["f"]["OR"] = std::bind(f_OR, _x);
            func_op["g"]["OR"] = std::bind(g_OR, _x);
        }

        int _runFunction(char *pipe_name, char *func_id, char *op_name) {
            std::cout << "(Worker) Connecting to the pipe..." << std::endl;

            // Open the named pipe
            HANDLE pipe = CreateFile(
                    pipe_name,
                    GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr,
                    CREATE_NEW,
                    FILE_ATTRIBUTE_NORMAL,
                    nullptr
            );

            if (pipe == INVALID_HANDLE_VALUE) {
                std::cout << "(Worker) Failed to connect to pipe" << std::endl;
                return 1;
            }

            std::cout << "(Worker) Writing data to pipe..." << std::endl;

            std::any buffer = func_op[func_id][op_name];

            DWORD numBytesWritten = 0;
            BOOL result = WriteFile(
                    pipe,
                    &buffer,
                    sizeof(std::any),
                    &numBytesWritten,
                    nullptr
            );

            if (result) {
                std::cout << "(Worker) Value was written: " << std::any_cast<char*>(buffer) << std::endl;
            } else {
                std::cout << "(Worker) Failed to write the data to a pipe." << std::endl;
            }

            CloseHandle(pipe);
            std::cout << "(Worker) Done" << std::endl;

            return 0;
        }

    };
} //namespace spos::lab1::utils

/*
argv:
    [1] operation_name,
    [2] function_id,
    [3] x_argument,
    [4] pipe_name
*/

int main(int argc, char *argv[]) {
    spos::lab1::utils::Worker worker = spos::lab1::utils::Worker(argv[3]);

    return worker._runFunction(argv[4], argv[2], argv[1]);
}
