#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-data.h>

#include "struct.h"

#include "digittrie.h"
#include "dir.c"
#include "exif.c"

// FIXME: working on this

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir_name> <file_ext>\n", argv[0]);

        return 1;
    }

    // DTNode *trie = dt_init(14); // like "YYYYMMDDHHMMSS"

    size_t f_count = ec_dir_count(argv[1], argv[2]);
    ECFile *f_arr = ec_dir_list(argv[1], argv[2], f_count);
    assert (f_arr != NULL);

    for (size_t i = 0; i < f_count; i++) {
        ec_exif_extract(&f_arr[i]);

        free(f_arr[i].name);
    }

    free(f_arr);

    // dt_destroy(trie);

    return 0;
}
