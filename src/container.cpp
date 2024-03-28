#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

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

template <typename... P>
int run(P... params)
{
    clearenv();
    char *args[] = {(char *)params..., (char *)0};
    return execvp(args[0], args);
}

int jail(void *args)
{
    run("/bin/sh");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    std::cout << "Hello, World! ( parent ) " << std::endl;

    clone(jail, stack_memory(), SIGCHLD, 0);
    wait(nullptr);
    return EXIT_SUCCESS;
}