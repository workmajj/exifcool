#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <libexif/exif-data.h>

/* macros */

#define EC_EXIF_IFD EXIF_IFD_0
#define EC_EXIF_TAG EXIF_TAG_DATE_TIME

#define EC_EXIF_TAG_BYTES 20 // per exif spec for datetime tags

/* util */

static int ec_file_filter(const struct dirent *ep, const char *ext)
{
    if (!ep || !ext) return 0;

    if (ep->d_type != DT_REG) return 0; // not a regular file

    int lenext = strlen(ext); // FIXME: strnlen?
    if (lenext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - lenext, ext, lenext) == 0);
}

/* exif */

static void ec_exif_print_date(const ExifData *ed)
{
    if (!ed) return;

    ExifEntry *ent = exif_content_get_entry(ed->ifd[EC_EXIF_IFD], EC_EXIF_TAG);
    if (!ent) return;

    char buf[EC_EXIF_TAG_BYTES];

    exif_entry_get_value(ent, buf, sizeof(buf));
    if (!buf) return;

    printf("%s\n", buf);
}

static void ec_exif_print(const struct dirent *ep)
{
    if (!ep) return;

    ExifData *ed;

    ed = exif_data_new_from_file(ep->d_name); // FIXME: need full file path
    if (!ed) {
        printf("couldn't get data from file %s\n", ep->d_name); // TODO: perror?
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
        printf("couldn't open dir %s\n", argv[1]); // TODO: perror?
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
