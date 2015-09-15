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

/*
void ec_buf_filter_digits(char *buf, const size_t size, char **str_ptr)
{
    assert(buf != NULL);
    assert(strnlen(buf, size) < size);
    assert(str_ptr != NULL);

    char *str = malloc(size);
    assert(str != NULL);
    memset(str, 0, size);

    for (size_t i = 0, j = 0; i < size; i++) {
        if (buf[i] >= EC_ASCII_0 && buf[i] <= EC_ASCII_9) {
            str[j] = buf[i];
            j++;
        }
    }

    *str_ptr = str;
}

void ec_exif_print_date(const ExifData *ed)
{
    assert(ed != NULL);

    ExifEntry *ent = exif_content_get_entry(ed->ifd[EC_EXIF_IFD], EC_EXIF_TAG);
    if (!ent) {
        printf("n/a");
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

void ec_exif_print(const char *f_name)
{
    assert(f_name != NULL);

    ExifData *ed = exif_data_new_from_file(f_name); // FIXME: need full path
    if (!ed) {
        printf("couldn't get exif data from %s\n", f_name);
        return;
    }

    printf("%s => ", f_name);
    ec_exif_print_date(ed);
    printf("\n");

    exif_data_unref(ed);
}
*/

void ec_exif_extract(ECFile *f)
{
    assert(f != NULL);

    ExifData *ed = exif_data_new_from_file(f->name); // FIXME: need full path
    if (!ed) return; // FIXME

    ExifEntry *ent = exif_content_get_entry(ed->ifd[EC_EXIF_IFD], EC_EXIF_TAG);
    if (!ent) {
        exif_data_unref(ed);
        return; // FIXME: tag doesn't exist
    }

    char buf[EC_EXIF_TAG_BYTES];
    exif_entry_get_value(ent, buf, sizeof(buf));
    assert(buf != NULL);

    printf("buf: %s\n", buf);

    char *tok;
    char *buf_ptr = buf;
    size_t i = 0;

    while ((tok = strsep(&buf_ptr, ": "))) {
        switch (i) {
        case 0:
            f->dt.year = atoi(tok);
            break;
        case 1:
            f->dt.month = atoi(tok);
            break;
        case 2:
            f->dt.day = atoi(tok);
            break;
        case 3:
            f->dt.hour = atoi(tok);
            break;
        case 4:
            f->dt.minute = atoi(tok);
            break;
        case 5:
            f->dt.second = atoi(tok);
            break;
        default:
            assert(0 && "not reached"); // malformed string
        }

        i++;
    }

    printf("dt.year: %d\n", f->dt.year);
    printf("dt.month: %d\n", f->dt.month);
    printf("dt.day: %d\n", f->dt.day);
    printf("dt.hour: %d\n", f->dt.hour);
    printf("dt.minute: %d\n", f->dt.minute);
    printf("dt.second: %d\n\n", f->dt.second);

    exif_data_unref(ed);
}

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

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir_name> <file_ext>\n", argv[0]);

        return 1;
    }

    // DTNode *trie = dt_init(EC_TRIE_STR_LEN);

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
