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
    int offset = argc - 1;            // mark the incoming new arguments read from stdin
    char buffer[512];                 // store input
    int lend, lbegin = 0;             // pointer to read a line
    while (read(0, buffer + lend, 1)) // read a single character at a time, but read(0,buffer,sizeof buffer) can only read 1 character, dont know reason yet
    {
        if (buffer[lend] != '\n') // move until lbegin--lend is a line of arguments
        {
            ++lend;
            continue;
        }
        int abegin, aend = lbegin; // pointer to read an argument, delimited by space.

        for (; aend <= lend; ++aend) // within the line, distill the argument
        {
            if (buffer[aend] != ' ' && buffer[aend] != '\n')
                continue;
            buffer[aend] = '\0';
            new_argv[offset] = malloc(strlen(buffer + abegin) + 1);
            strcpy(new_argv[offset++], buffer + abegin);
            abegin = ++aend;
        }
        lbegin = ++lend;      // move line pointer towards next line
        new_argv[offset] = 0; // argv is terminated by null pointer
        if (fork() == 0)
        {
            exec(new_argv[0], new_argv);
            fprintf(2, "exec %s faile\n", new_argv[0]);
            exit(1);
        }
        else
        {
            wait(0); // wait for the process of the line end, then process next line
        }
        // free memory
        for (int i = argc - 1; i < offset; i++)
            free(new_argv[i]);

        offset = argc - 1; // restore the offset to mark new incoming arguments
    }

    exit(0);
}
