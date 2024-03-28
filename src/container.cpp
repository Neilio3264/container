#include <iostream>
#include <sys/wait.h>

char *stack_memory()
{
    const int stackSize = 65536;
    auto *stack = new (std::nothrow) char[stackSize];

    if (!stack)
    {
        std::cerr << "Cannot allocate memory " << std::endl;
        exit(EXIT_FAILURE);
    }

    return stack + stackSize;
}

int jail(void *args)
{
    std::cout << "Hello !! ( child ) " << std::endl;
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    std::cout << "Hello, World! ( parent ) " << std::endl;

    clone(jail, stack_memory(), SIGCHLD, 0);
    wait(nullptr);
    return EXIT_SUCCESS;
}