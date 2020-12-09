#include <iostream>
#include "src/manager.hpp"

int main() {

    freopen("output.txt", "w", stdout);

    spos::lab1::Manager test("OR", 1);
    test.run();

    std::cout<<test.getResult().value();

    return 0;
}
