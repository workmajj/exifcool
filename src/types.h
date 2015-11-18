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
