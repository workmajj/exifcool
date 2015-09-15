#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-data.h>

typedef struct ec_datetime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} ECDateTime;

typedef struct ec_file {
    char *name;
    struct ec_datetime dt;
} ECFile;

#include "digittrie.h"
#include "dir.c"
#include "exif.c"

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
        // ec_exif_print(f_arr[i].name);

        ec_exif_extract(&f_arr[i]);

        free(f_arr[i].name);
    }

    free(f_arr);

    // dt_destroy(trie);

    return 0;
}
