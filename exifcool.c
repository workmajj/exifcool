#include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-data.h>

#include "trie.h"

// #define EC_TRIE_STR_LEN 14 // for strings like "YYYYMMDDHHMMSS"

#define EC_EXIF_IFD EXIF_IFD_0
#define EC_EXIF_TAG EXIF_TAG_DATE_TIME
#define EC_EXIF_TAG_BYTES 20 // per exif spec for datetime tags

#define EC_ASCII_0 48
#define EC_ASCII_9 57

/* util */

static void ec_buf_filter_digits(char *buf, const size_t size, char **strptr)
{
    assert(buf != NULL && strnlen(buf, size) < size);
    assert(*strptr != NULL && strptr != NULL);

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

/*
static void ec_exif_print(const struct dirent *ep)
{
    assert(ep != NULL);

    ExifData *ed = exif_data_new_from_file(ep->d_name); // FIXME: need full path
    if (!ed) {
        printf("couldn't get exif data from %s\n", ep->d_name);
        return;
    }

    printf("%s => ", ep->d_name);
    ec_exif_print_date(ed);
    printf("\n");

    exif_data_unref(ed);
}
*/

/* main */





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

static size_t ec_dir_filter(const struct dirent *ep, const char *file_ext)
{
    assert(ep != NULL && file_ext != NULL);

    if (ep->d_type != DT_REG) return 0; // ignore non-regular files

    size_t lenext = strlen(file_ext);
    if (lenext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - lenext, file_ext, lenext) == 0);
}

/*
static void ec_dir_scan(DIR *dirp, const char *file_ext, ec_file_t **file_list)
{
    struct dirent *ep;
    // struct ec_file_t *fp;

    while ((ep = readdir(dirp))) {
        if (!ec_dir_filter(ep, file_ext)) continue;

        ec_exif_print(ep->d_name);
    }
}
*/






#ifdef _DARWIN_FEATURE_64_BIT_INODE
#define EC_FILE_NAME_BYTES 1024
#else
#define EC_FILE_NAME_BYTES 256
#endif // _DARWIN_FEATURE_64_BIT_INODE

typedef struct ECFile {
    char name[EC_FILE_NAME_BYTES]; // TODO: make size dynamic
} ec_file_t;

static size_t ec_dir_scan(const char *dir_name, const char *file_ext, ec_file_t **files_ptr)
{
    assert(dir_name != NULL);
    assert(file_ext != NULL);

    DIR *dirp = opendir(dir_name);
    if (!dirp) {
        printf("couldn't open dir %s\n", dir_name);
        exit(1);
    }

    struct dirent *ep;
    size_t count_file = 0;

    while ((ep = readdir(dirp))) {
        if (!ec_dir_filter(ep, file_ext)) continue;

        count_file++;
    }

    rewinddir(dirp);

    if (count_file == 0) {
        closedir(dirp);
        printf("no files with ext %s in dir %s\n", file_ext, dir_name);
        exit(0);
    }

    ec_file_t *files = malloc(count_file * sizeof(ec_file_t));
    assert(files != NULL);

    ec_file_t *filep = files;
    size_t count_check = 0;

    while ((ep = readdir(dirp))) {
        if (!ec_dir_filter(ep, file_ext)) continue;

        // TODO: make size dynamic
        memset(filep->name, 0, EC_FILE_NAME_BYTES);
        strlcpy(filep->name, ep->d_name, EC_FILE_NAME_BYTES);

        filep++;
        count_check++;
    }

    closedir(dirp);

    if (count_check != count_file) {
        free(files);
        printf("dir %s has been modified\n", dir_name);
        exit(1);
    }

    *files_ptr = files;

    return count_file;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <file_ext>\n", argv[0]);
        return 1;
    }

    ec_file_t *files;
    size_t file_count = ec_dir_scan(argv[1], argv[2], &files);
    assert(files != NULL);

    for (size_t i = 0; i < file_count; i++) {
        printf("%s\n", files[i].name);
    }

    free(files);

    /*
    DIR *dirp = opendir(argv[1]);
    if (!dirp) {
        printf("couldn't open dir %s\n", argv[1]);
        return 1;
    }

    ec_file_t *file_list;
    ec_dir_scan(dirp, argv[2], &file_list);
    free(file_list);

    closedir(dirp);
    */

    return 0;
}





/*
int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <ext>\n", argv[0]);

        return 1;
    }

    DIR *dp = opendir(argv[1]);
    if (!dp) {
        printf("couldn't open dir %s\n", argv[1]);

        return 1;
    }

    // dt_node_t *trie = dt_init(EC_TRIE_STR_LEN);

    struct dirent *ep;
    while ((ep = readdir(dp))) {
        if (!ec_file_filter(ep, argv[2])) continue;

        ec_exif_print(ep);
    }

    // dt_destroy(trie);

    closedir(dp);

    return 0;
}
*/
