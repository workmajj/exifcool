#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exifcool.h"

size_t ec_dir_filter(const struct dirent *ep, const char *f_ext)
{
    assert(ep != NULL);
    assert(f_ext != NULL);

    if (ep->d_type != DT_REG) return 0; // ignore non-regular files

    size_t len_ext = strlen(f_ext);
    if (len_ext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - len_ext, f_ext, len_ext) == 0);
}

size_t ec_dir_count(const char *dir, const char *f_ext)
{
    assert(dir != NULL);
    assert(f_ext != NULL);

    DIR *dirp = opendir(dir);
    if (!dirp) {
        printf("couldn't open dir %s\n", dir);
        exit(1);
    }

    size_t f_count = 0;

    struct dirent *ep;
    while ((ep = readdir(dirp))) {
        if (ec_dir_filter(ep, f_ext)) f_count++;
    }

    closedir(dirp);

    if (f_count == 0) {
        printf("no files with ext %s in dir %s\n", f_ext, dir);
        exit(0);
    }

    return f_count;
}

ECFile *ec_dir_list(const char *dir, const char *f_ext, const size_t f_count)
{
    assert(dir != NULL);
    assert(f_ext != NULL);
    assert(f_count > 0);

    DIR *dirp = opendir(dir);
    if (!dirp) {
        printf("couldn't open dir %s\n", dir);
        exit(1);
    }

    ECFile *f_arr = malloc(f_count * sizeof(ECFile));
    assert(f_arr != NULL);

    size_t chk_count = 0;

    ECFile *f_ptr = f_arr;

    struct dirent *ep;
    while ((ep = readdir(dirp))) {
        if (!ec_dir_filter(ep, f_ext)) continue;

        f_ptr->name = malloc(ep->d_namlen * sizeof(char));
        assert(f_ptr->name != NULL);
        strlcpy(f_ptr->name, ep->d_name, ep->d_namlen + 1);

        f_ptr++;
        chk_count++;
    }

    closedir(dirp);

    if (chk_count != f_count) {
        for (size_t i = 0; i < f_count; i++) free(f_arr[i].name);
        free(f_arr);

        printf("dir %s has been modified\n", dir);
        exit(1);
    }

    return f_arr;
}
