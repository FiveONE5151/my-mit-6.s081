#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

/**
 * Write a simple version of the UNIX find program: find all the files in a directory tree with a specific name. Your solution should be in the file user/find.c.
 */

int find(char *path, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de; // entry of a director
    struct stat st;   // info of a file

    // open the designated path
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }
    // read the status of the designated file into st
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE:
        p = path + strlen(path);
        // let p represent the file name
        while (*p != '/')
            p--;
        p += 1;

        if (strcmp(p, target) == 0)
            printf("%s\n", path);
        break;

    case T_DIR:
        strcpy(buf, path);
        p = buf + strlen(buf); // move p to the end of buf
        *p++ = '/';

        read(fd, &de, sizeof(de));                      // skip .
        read(fd, &de, sizeof(de));                      // skip ..
        while (read(fd, &de, sizeof(de)) == sizeof(de)) // read each entry of this director into de
        {
            if (de.inum == 0) // invalid file
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);

            break;
        }
        close(fd);
        return 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: find dirpath filepath\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
