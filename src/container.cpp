#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>

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

template <typename Function>
void clone_process(Function &&function, int flags)
{
    auto pid = clone(function, stack_memory(), flags, 0);
    wait(nullptr);
}

template <typename... P>
int run(P... params)
{
    char *args[] = {(char *)params..., (char *)0};
    return execvp(args[0], args);
}

void setup_variables()
{
    clearenv();
    setenv("TERM", "xterm-256color", 0);
    setenv("PATH", "bin/:/sbin/:usr/bin:/usr/sbin", 0);
}

void setup_root(const char *folder)
{
    chroot(folder);
    chdir("/");
}

int jail(void *args)
{
    std::cout << "Hello, World! ( child ) " << std::endl;
    std::cout << "Child ID: " << getpid() << std::endl;

    setup_variables();
    setup_root("./utils/root/");

    mount("proc", "/proc", "proc", 0, 0);

    auto runThis = [](void *args) -> int
    { run("/bin/sh"); };

    clone_process(runThis, SIGCHLD);

    umount("/proc");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    std::cout << "Hello, World! ( parent ) " << std::endl;
    std::cout << "Parent ID: " << getpid() << std::endl;

    clone_process(jail, CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD);

    return EXIT_SUCCESS;
}