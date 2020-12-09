//
// Created by vbory on 12/9/2020.
//

#ifndef LAB_1_WORKER_HPP
#define LAB_1_WORKER_HPP

#include "demofuncs.hpp"
#include <iostream>
#include <windows.h>
#include <functional>

namespace spos::lab1::utils {
class Worker {
    char *_func_id;
public:
    explicit Worker(char* func_id) : _func_id(func_id){}

    bool getFunctionResult(int x);

    static int sendResult(char *pipe_name, char* buffer, unsigned int numOfBytes);
};

static char* dummyCStrCast(auto arg);

static int dummyIntCast(char* arg);
}//namespace spos::lab1::utils


#endif //LAB_1_WORKER_HPP
