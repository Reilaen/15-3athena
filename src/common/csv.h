#ifndef _CSV__H_
#define _CSV__H_

#include "../common/cbasetypes.h"
#include "../common/mmo.h"
#include "../common/showmsg.h"

#define CSV_MAX_FIELDS      64
#define CSV_MAX_LINE_LEN    4096
#define CSV_MAX_STR_LEN     1024
#define CSV_DEFAULT_DELIM   ','

#define csv_get_int(res, idx) csv_get_int_raw(res, idx, 0)

#define csv_get_str(res, idx) csv_get_str_raw(res, idx, "")
#define csv_get_str_buffer(res, idx) _csv_get_str(res, idx, "")
#define _csv_get_str(res, idx, def) strncpy((char[CSV_MAX_STR_LEN]){0}, csv_get_str_raw(res, idx, def), CSV_MAX_STR_LEN - 1)

enum csv_parse_flags {
    CSV_FLAG_NONE           = 0x00,
    CSV_FLAG_TRIM_WHITESPACE= 0x01, // Remove leading/trailing whitespace on each field
    CSV_FLAG_ALLOW_QUOTES   = 0x02, // Allow quotes in fields
    CSV_FLAG_BRACE_AWARE    = 0x04, // {}-Dont separate on {}
    CSV_FLAG_EMPTY_AS_NULL  = 0x10, // Empty fields as NULL-Pointer
};

typedef struct csv_parse_result {
    int field_count; // Found fields
    char *fields[CSV_MAX_FIELDS]; // Pointers to the original line (zero-terminated)
    int error_code; // Value< 0 = Error
    const char *error_msg;
} csv_result_t;

// Error-Codes
enum csv_error {
    CSV_OK                  = 0,
    CSV_ERR_TOO_FEW_FIELDS  = -1,
    CSV_ERR_TOO_MANY_FIELDS = -2,
    CSV_ERR_UNCLOSED_BRACE  = -3,
    CSV_ERR_UNCLOSED_QUOTE  = -4,
    CSV_ERR_LINE_TOO_LONG   = -5,
    CSV_ERR_EMPTY_LINE      = -6,
};

/**
 * Parse a line
 * @param line          Line to parse
 * @param result        Result-Struct
 * @param max_fields    Expected number of fields
 * @param delimiter     Delimiter (default: ',')
 * @param flags         Combined flags (see enum csv_parse_flags)
 * @return              CSV_OK if successful else error code
 */
int csv_parse_line(char *line, csv_result_t *result, int max_fields, char delimiter, uint32_t flags);

/**
 * Read a field as int
 */
int csv_get_int_raw(const csv_result_t *res, int idx, int default_val);

/**
 * Read field as string (NULL-safe)
 */
const char* csv_get_str_raw(const csv_result_t* res, int idx, const char* def);

static char* find_comment_marker(char *str, uint32 flags);

#endif // _CSV__H_