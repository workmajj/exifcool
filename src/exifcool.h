#ifndef EC_EXIFCOOL_H
#define EC_EXIFCOOL_H

typedef struct ECDateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} ECDateTime;

typedef struct ECFile {
    char *name;
    struct ECDateTime dt;
} ECFile;

size_t ec_dir_count(const char *dir, const char *f_ext);
ECFile *ec_dir_list(const char *dir, const char *f_ext, const size_t f_count);

void ec_exif_extract(ECFile *f);

#endif // EC_EXIFCOOL_H
