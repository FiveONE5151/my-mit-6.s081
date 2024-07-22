#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// static int stdin = 0;
int main(int argc, char *argv[])
{
    if (argc >= MAXARG)
    {
        fprintf(2, "Too many arguments\n");
        exit(1);
    }

    char *new_argv[MAXARG];
    // put initial arguments into new_argv
    for (int i = 1; i < argc; i++)
    {
        new_argv[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(new_argv[i - 1], argv[i]);
    }
    char input_argv[512];
    int n = read(0, input_argv, sizeof(input_argv)); // number of bytes read from input
    if (n == -1)
    {
        fprintf(2, "Error in reading standard input\n");
        exit(1);
    }
    // read a line of arguments
    int offset = argc - 1;
    char *p = input_argv;
    for (int i = 0; i < n; i++)
    {
        if (input_argv[i] != '\n')
            continue;
        input_argv[i] = '\0';
        new_argv[offset++] = p;
        p = input_argv + i + 1;
        if (fork() == 0)
        {
            exec(new_argv[0], new_argv);
            fprintf(2, "exec %s failed.\n", new_argv[0]);
            exit(1);
        }
        else
        {
            wait(0);
        }
    }

    exit(0);
}
