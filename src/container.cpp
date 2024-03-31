#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#define CGROUP_FOLDER "/sys/fs/cgroup/pids/container/"
#define concat(a, b) (a "" b)

int TRY(int status, const char *msg)
{
    if (status == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return status;
}

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

void write_rule(const char *path, const char *value)
{
    int fp = open(path, O_WRONLY | O_APPEND);
    write(fp, value, strlen(value));
    close(fp);
}

void limitProcessCreation()
{
    mkdir(CGROUP_FOLDER, S_IRUSR | S_IWUSR);

    const char *pid = std::to_string(getpid()).c_str();

    write_rule(concat(CGROUP_FOLDER, "cgroup.procs"), pid);
    write_rule(concat(CGROUP_FOLDER, "notify_on_release"), "1");
    write_rule(concat(CGROUP_FOLDER, "pids.max"), "5");
}

template <typename Function>
void clone_process(Function &&function, int flags)
{
    TRY(clone(function, stack_memory(), flags, 0), "clone");
    wait(nullptr);
}

template <typename... P>
int run(P... params)
{
    char *args[] = {(char *)params..., (char *)0};
    return execvp(args[0], args);
}

void setHostName(std::string hostname)
{
    sethostname(hostname.c_str(), hostname.size());
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

    limitProcessCreation();
    setup_variables();
    setup_root("./utils/root/");

    mount("proc", "/proc", "proc", 0, 0);

    auto runThis = [](void *args) -> int
    { return run("/bin/sh"); };

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