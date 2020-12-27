//
// Created by vbory on 12/9/2020.
//

#ifndef LAB_1_WORKER_HPP
#define LAB_1_WORKER_HPP

#include "demofuncs.hpp"
#include <iostream>
#include <windows.h>

namespace spos::lab1::utils {
class Worker {
    char *_func_id;
    HANDLE _pipe;
public:
    explicit Worker(char* func_id) : _func_id(func_id){}

    bool getFunctionResult(int x);

    int connectPipe(char *pipe_name);

    int sendResult(char* buffer, unsigned int numOfBytes);
};

char* dummyCStrCast(bool arg);
}//namespace spos::lab1::utils


#endif //LAB_1_WORKER_HPP
