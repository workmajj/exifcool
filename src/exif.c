#define EC_EXIF_IFD EXIF_IFD_0
#define EC_EXIF_TAG EXIF_TAG_DATE_TIME

#define EC_EXIF_TAG_BYTES 20 // per exif spec for datetime tags

// FIXME: working on this section

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
