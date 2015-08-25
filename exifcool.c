#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include <libexif/exif-data.h>

int _ec_file_filter(const struct dirent *ep, const char *ext)
{
    if (!ep || !ext) return 0;

    if (ep->d_type != DT_REG) return 0; // not a regular file

    int lenext = strlen(ext); // FIXME: strnlen?
    if (lenext > ep->d_namlen) return 0;

    return (strncmp(ep->d_name + ep->d_namlen - lenext, ext, lenext) == 0);
}

void _ec_exif_print_date(const ExifData *ed)
{
    if (!ed) return;

    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0],
        EXIF_TAG_DATE_TIME);
    if (!entry) return;

    char buf[1024];
    exif_entry_get_value(entry, buf, sizeof(buf));

    if (*buf) {
        printf("%s: %s\n", exif_tag_get_name_in_ifd(EXIF_TAG_DATE_TIME,
            EXIF_IFD_0), buf);
    }
}

void _ec_exif_print(const struct dirent *ep)
{
    if (!ep) return;

    ExifData *ed;

    ed = exif_data_new_from_file(ep->d_name);
    if (!ed) {
        printf("couldn't get exif data from file %s\n", ep->d_name); // TODO: perror?
        return;
    }

    _ec_exif_print_date(ed);

    exif_data_unref(ed);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <dir> <ext>\n", argv[0]);
        return 1;
    }

    DIR *dp;

    dp = opendir(argv[1]);
    if (!dp) {
        printf("couldn't open directory %s\n", argv[1]); // TODO: perror?
        return 1;
    }

    struct dirent *ep;
    while ((ep = readdir(dp))) {
        if (!_ec_file_filter(ep, argv[2])) continue;

        _ec_exif_print(ep);
    }

    closedir(dp);

    return 0;
}
