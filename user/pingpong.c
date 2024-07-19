#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

// Write a program that uses UNIX system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. The parent should send a byte to the child; the child should print "<pid>: received ping", where <pid> is its process ID, write the byte on the pipe to the parent, and exit; the parent should read the byte from the child, print "<pid>: received pong", and exit. Your solution should be in the file user/pingpong.c.
int p[2];
char buffer[256];

int main(int argc, char *argv[])
{
    pipe(p);
    if (fork() == 0)
    {
        int n = read(p[0], buffer, 1);
        if (n <= 0)
        {
            fprintf(1, "Doesn't received from parent\n");
            exit(1);
        }
        close(p[0]);
        int childPid = getpid();
        printf("%d: received ping\n", childPid);
        write(p[1], "b", 1);
        close(p[1]);
        exit(0);
    }
    else
    {
        write(p[1], "a", 1);
        close(p[1]);
        int n = read(p[0], buffer, 1);
        if (n <= 0)
        {
            fprintf(1, "Doesn't received from child\n");
            exit(1);
        }
        int pid = getpid();
        printf("%d: received pong\n", pid);
        close(p[0]);
        exit(0);
    }
}