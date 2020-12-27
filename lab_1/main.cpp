#include <iostream>
#include "src/manager.hpp"

int main() {

    freopen("output.txt", "w", stdout);

    spos::lab1::ManagerBase test(0);
    test.run();

    std::cout<<"____main after manager___"<<std::endl;

    std::optional <bool> res = test.getResult();
    if (res)
        std::cout<<test.getResult().value();
    else
        std::cout<<"sdkjlkhfhkeiuhfiueygf"<<std::endl;

    return 0;
}
