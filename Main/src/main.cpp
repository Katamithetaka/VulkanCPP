#include <vulkan/vulkan.h>
#include <Engine.hpp>
#include <iostream>

int main() {

    int a = Engine::add(1, 2);
    std::cout << a << std::endl;
    return a;
}