//
// Created by vbory on 12/9/2020.
//

#include "worker.hpp"

namespace spos::lab1::utils{
char* dummyCStrCast(bool b) {
    char *tmp_buffer = (char *) &b;//cast func result into char*
    char *buffer = new char[sizeof(b)];
    for (int i = 0; i < sizeof(b); i++) {
        buffer[i] = tmp_buffer[i];
    }
    return buffer;
}

bool Worker::getFunctionResult(int x) {
    std::cout<<"Computing func result..."<<std::endl;
    if (_func_id[0] == 'f')
        return demo::f_func<demo::OR>(x);
    else if (_func_id[0] == 'g')
        return demo::g_func<demo::OR>(x);
    else
        throw std::invalid_argument("Invalid function id in worker");
}

int Worker::sendResult(char* pipe_name, char* buffer, unsigned int numOfBytes) {
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

    std::cout << "(Worker) Writing data to pipe..." << std::endl;

    DWORD numBytesWritten = 0;
    BOOL isWritten = WriteFile(
            pipe,
            buffer,
            numOfBytes,
            &numBytesWritten,
            nullptr
    );

    if (isWritten) {
        std::cout << "(Worker) Value was written: " << bool(buffer[0]) << "**" << numBytesWritten << std::endl;
    } else {
        std::cout << "(Worker) Failed to write the data to a pipe." << std::endl;
    }

    CloseHandle(pipe);
    std::cout << "(Worker) Done" << std::endl;

    return 0;
}

}
