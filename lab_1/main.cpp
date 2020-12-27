#include <iostream>
#include "src/manager.hpp"

int main() {

    freopen("output.txt", "w", stdout);

    spos::lab1::ManagerBase test(2);
    test.run();

    std::optional <bool> res = test.getResult();
    if (res)
        std::cout<<test.getResult().value();
    else
        std::cout<<"__Result's not computed____"<<std::endl;

    return 0;
}
