#include <stdio.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("usage: %s <dir>\n", argv[0]);
        return 1;
    }

    DIR *dp;

    dp = opendir(argv[1]);
    if (NULL == dp) {
        printf("couldn't open directory %s\n", argv[1]); // TODO: perror?
        return 1;
    }

    struct dirent *ep;
    while ((ep = readdir(dp))) {
        printf("%s\n", ep->d_name);
    }

    closedir(dp);
    return 0;
}
