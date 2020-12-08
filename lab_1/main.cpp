#include <iostream>
#include "src/manager.hpp"

int main() {

    freopen("output.txt", "w", stdout);

    spos::lab1::Manager test("OR", 2);
    test.run();

    return 0;
}
