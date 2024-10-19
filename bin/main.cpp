#include "include/interface.h"
#include <stdexcept>

int main() {

    Interface interface{};
    try{
        interface.RunInterface();
    } catch (std::exception& e) {
        std::cerr << e.what();
    }

    return 0;
}