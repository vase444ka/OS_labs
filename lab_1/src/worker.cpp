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
    std::cout<<"(Worker) Computing func result..."<<std::endl;
    if (_func_id[0] == 'f')
        return demo::f_func<demo::OR>(x);
    else if (_func_id[0] == 'g')
        return demo::g_func<demo::OR>(x);
    else
        throw std::invalid_argument("Invalid function id in worker");
}

int Worker::connectPipe(char *pipe_name) {
    std::cout << "(Worker) Connecting to the _pipe..." << std::endl;

    // Open the named _pipe
    _pipe = CreateFile(
            pipe_name,
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            nullptr,
            CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    if (_pipe == INVALID_HANDLE_VALUE) {
        std::cout << "(Worker) Failed to connect to _pipe" << std::endl;
        return 1;
    }
    return 0;
}

int Worker::sendResult(char* buffer, unsigned int numOfBytes) {
    std::cout << "(Worker) Writing data to _pipe..." << std::endl;

    DWORD numBytesWritten = 0;
    BOOL isWritten = WriteFile(
            _pipe,
            buffer,
            numOfBytes,
            &numBytesWritten,
            NULL
    );

   if (isWritten) {
        std::cout << "(Worker) Value written: " << bool(buffer[0]) << "___size(bytes):_____" << numBytesWritten << std::endl;
    } else {
        std::cout << "(Worker) Failed to write the data to a _pipe. "<<GetLastError()<< std::endl;
    }

    CloseHandle(_pipe);
    std::cout << "(Worker) Done" << std::endl;

    return 0;
}



}
