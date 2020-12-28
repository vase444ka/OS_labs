#include <iostream>
#include "src/manager.hpp"

int main() {
    int x;
    std::cin>>x;

    spos::lab1::Manager test(x);
    test.run();
    test.printResult();

    return 0;
}
