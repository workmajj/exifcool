#include <assert.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-data.h>

/* macros */

#define EC_EXIF_IFD EXIF_IFD_0
#define EC_EXIF_TAG EXIF_TAG_DATE_TIME

#define EC_EXIF_TAG_BYTES 20 // per exif spec for datetime tags

/* DigitTrie */

// TODO: move to external header+source files
// TODO: set func storage classes

// all keys are uniform-length strings of digits (e.g., "20140830143607")

size_t dt_length = 0; // uninitialized value

typedef struct DigitTrieNode {
    struct DigitTrieNode *children[10]; // digits 0-9
    size_t count;
} dt_node_t;

dt_node_t *dt_alloc()
{
    dt_node_t *n = malloc(sizeof(dt_node_t));
    assert(n != NULL);

    for (size_t i = 0; i < 10; i++) n->children[i] = NULL;

    return n;
}

dt_node_t *dt_init(const size_t length)
{
    assert(length > 0 && length < SIZE_MAX);
    dt_length = length;

    return dt_alloc();
}

void dt_free(dt_node_t *n)
{
    assert(n != NULL);

    for (size_t i = 0; i < 10; i++) {
        if (n->children[i] != NULL) dt_free(n->children[i]);
    }

    free(n);
}

void dt_destroy(dt_node_t *root)
{
    assert(dt_length > 0); // init-ed
    dt_length = 0;

    dt_free(root);
}

int dt_ctoi(const char *c)
{
    char a[2] = {c[0], '\0'};

    return atoi(a);
}

void dt_set(dt_node_t *root, const char *digit_str, const size_t count)
{
    assert(dt_length > 0); // init-ed

    assert(root != NULL);
    assert(digit_str != NULL);
    assert(count < SIZE_MAX);

    size_t len = strnlen(digit_str, dt_length + 1);
    assert(len == dt_length);

    dt_node_t *curr = root;

    for (size_t i = 0; i < len; i++) {
        int idx = dt_ctoi(&digit_str[i]);

        if (!curr->children[idx]) curr->children[idx] = dt_alloc();

        curr = curr->children[idx];
    }

    curr->count = count;
}

size_t dt_get(dt_node_t *root, const char *digit_str)
{
    assert(dt_length > 0); // init-ed

    assert(root != NULL);
    assert(digit_str != NULL);

    size_t len = strnlen(digit_str, dt_length + 1);
    assert(len == dt_length);

    dt_node_t *curr = root;

    for (size_t i = 0; i < len; i++) {
        int idx = dt_ctoi(&digit_str[i]);

        if (!curr->children[idx]) return 0; // not yet set

        curr = curr->children[idx];
    }

    return curr->count;
}

/* file */

static int ec_file_filter(const struct dirent *ep, const char *ext)
{
    assert(ep != NULL && ext != NULL);

    if (ep->d_type != DT_REG) return 0; // not a regular file

    int lenext = strlen(ext);
    if (lenext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - lenext, ext, lenext) == 0);
}

/* exif */

static void ec_exif_print_date(const ExifData *ed)
{
    assert(ed != NULL);

    ExifEntry *ent = exif_content_get_entry(ed->ifd[EC_EXIF_IFD], EC_EXIF_TAG);
    assert(ent != NULL);

    char buf[EC_EXIF_TAG_BYTES];

    exif_entry_get_value(ent, buf, sizeof(buf));
    assert(buf != NULL);

    printf("%s\n", buf);
}

static void ec_exif_print(const struct dirent *ep)
{
    assert(ep != NULL);

    ExifData *ed;

    ed = exif_data_new_from_file(ep->d_name); // FIXME: need full file path
    if (!ed) {
        printf("couldn't get exif data from %s\n", ep->d_name);
        return;
    }

    printf("%s => ", ep->d_name);
    ec_exif_print_date(ed);

    exif_data_unref(ed);
}

/* main */

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <ext>\n", argv[0]);
        return 1;
    }

    DIR *dp;

    dp = opendir(argv[1]);
    if (!dp) {
        printf("couldn't open dir %s\n", argv[1]);
        return 1;
    }

    struct dirent *ep;
    while ((ep = readdir(dp))) {
        if (!ec_file_filter(ep, argv[2])) continue;

        ec_exif_print(ep);
    }

    closedir(dp);
    return 0;
}
