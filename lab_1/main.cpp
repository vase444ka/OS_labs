#include <iostream>
#include "src/manager.hpp"

int main() {
    spos::lab1::Manager test(5);
    test.run();
    test.printResult();
    return 0;
}
