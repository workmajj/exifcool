#include <stdio.h>
#include <dirent.h>
#include <string.h>

int _ec_file_filter(const struct dirent *ep, const char *ext)
{
    if (!ep || !ext) return 0;

    if (ep->d_type != DT_REG) return 0; // not a regular file

    int lenext = strlen(ext); // FIXME: strnlen?
    if (lenext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - lenext, ext, lenext) == 0);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <ext>\n", argv[0]);
        return 1;
    }

    DIR *dp;

    if (!(dp = opendir(argv[1]))) {
        printf("couldn't open directory %s\n", argv[1]); // TODO: perror?
        return 1;
    }

    struct dirent *ep;
    while ((ep = readdir(dp))) {
        if (!_ec_file_filter(ep, argv[2])) continue;

        printf("%s\n", ep->d_name);
    }

    closedir(dp);

    return 0;
}
