#include "worker.hpp"
/*
argv:
    [1] function_id,
    [2] x_argument,
    [3] pipe_name
*/

int main(int argc, char *argv[]) {
    auto worker = spos::lab1::utils::Worker(argv[1]);

    int x_arg = atoi(argv[2]);

    bool func_res = worker.getFunctionResult(x_arg);
    std::cout<<argv[1]<<"(Worker) func result computed"<<std::endl;

    char* buffer = spos::lab1::utils::dummyCStrCast(func_res);
    return spos::lab1::utils::Worker::sendResult(argv[3], buffer, sizeof(func_res));
}
