#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (argc != 2) // sleep 2 counts as 2 arguments, the argv[1] stands for "2"
    {
        fprintf(2, "Enter one argument only.");
        exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}