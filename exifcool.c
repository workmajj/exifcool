#include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-data.h>

// #include "trie.h"

#define EC_ASCII_0 48
#define EC_ASCII_9 57

#define EC_EXIF_IFD EXIF_IFD_0
#define EC_EXIF_TAG EXIF_TAG_DATE_TIME
#define EC_EXIF_TAG_BYTES 20 // per exif spec for datetime tags

// #define EC_TRIE_STR_LEN 14 // like "YYYYMMDDHHMMSS"

typedef struct ECFile {
    char *name;
} ec_file_t;

/* util */

static void ec_buf_filter_digits(char *buf, const size_t size, char **strptr)
{
    assert(buf != NULL && strnlen(buf, size) < size);
    assert(strptr != NULL);

    char *str = malloc(size);
    assert(str != NULL);
    memset(str, 0, size);

    size_t i, j = 0;

    for (i = 0; i < size; i++) {
        if (buf[i] >= EC_ASCII_0 && buf[i] <= EC_ASCII_9) {
            str[j] = buf[i];
            j++;
        }
    }

    *strptr = str;
}

/* exif */

static void ec_exif_print_date(const ExifData *ed)
{
    assert(ed != NULL);

    ExifEntry *ent = exif_content_get_entry(ed->ifd[EC_EXIF_IFD], EC_EXIF_TAG);
    if (!ent) {
        printf("[n/a]");
        return;
    }

    char buf[EC_EXIF_TAG_BYTES];
    exif_entry_get_value(ent, buf, sizeof(buf));
    assert(buf != NULL);

    char *str;
    ec_buf_filter_digits(buf, sizeof(buf), &str);
    assert(str != NULL);

    printf("%s", str);

    free(str);
}

static void ec_exif_print(const char *file_name)
{
    assert(file_name != NULL);

    ExifData *ed = exif_data_new_from_file(file_name); // FIXME: need full path
    if (!ed) {
        printf("couldn't get exif data from %s\n", file_name);
        return;
    }

    printf("%s => ", file_name);
    ec_exif_print_date(ed);
    printf("\n");

    exif_data_unref(ed);
}

/* dir */

static size_t ec_dir_filter(const struct dirent *ep, const char *f_ext)
{
    assert(ep != NULL);
    assert(f_ext != NULL);

    if (ep->d_type != DT_REG) return 0; // ignore non-regular files

    size_t len_ext = strlen(f_ext);
    if (len_ext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - len_ext, f_ext, len_ext) == 0);
}

static size_t ec_dir_count(const char *dir, const char *f_ext)
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

static ec_file_t *ec_dir_list(const char *dir, const char *f_ext,
    const size_t f_count)
{
    assert(dir != NULL);
    assert(f_ext != NULL);
    assert(f_count > 0);

    DIR *dirp = opendir(dir);
    if (!dirp) {
        printf("couldn't open dir %s\n", dir);
        exit(1);
    }

    ec_file_t *f_arr = malloc(f_count * sizeof(ec_file_t));
    assert(f_arr != NULL);

    ec_file_t *f_ptr = f_arr;
    size_t chk_count = 0;

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

/* main */

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <file_ext>\n", argv[0]);

        return 1;
    }

    // dt_node_t *trie = dt_init(EC_TRIE_STR_LEN);

    size_t f_count = ec_dir_count(argv[1], argv[2]);
    ec_file_t *f_arr = ec_dir_list(argv[1], argv[2], f_count);
    assert (f_arr != NULL);

    for (size_t i = 0; i < f_count; i++) {
        ec_exif_print(f_arr[i].name);

        free(f_arr[i].name);
    }

    free(f_arr);

    // dt_destroy(trie);

    return 0;
}
