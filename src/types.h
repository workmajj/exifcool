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
