#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void die(const char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char **argv)
{
    pid_t pid = fork();
    if (pid < 0) {
        die("fork");
    } else if (pid == 0) {
        // Child process
        exit(0);
    } else {
        // Parent process
        printf("Child PID = %d\n", pid);
        sleep(30);

        if (waitpid(pid, NULL, 0) < 0)
            die("waitpid");
    }
    return 0;
}
