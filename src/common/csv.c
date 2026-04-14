#include "csv.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static char* trim_whitespace(char *str) {
    if (!str) return str;
    while (ISSPACE(*str)) str++;
    if (*str == '\0') return str;
    char *end = str + strlen(str) - 1;
    while (end > str && ISSPACE(*end)) *end-- = '\0';
    return str;
}

int csv_parse_line(char *line, csv_result_t *result, const int max_fields, const char delimiter, const uint32_t flags) {
    if (!line || !result) return CSV_ERR_EMPTY_LINE;

    memset(result, 0, sizeof(csv_result_t));

    if (strlen(line) > CSV_MAX_LINE_LEN) {
        result->error_code = CSV_ERR_LINE_TOO_LONG;
        result->error_msg = "Line too long";
        return CSV_ERR_LINE_TOO_LONG;
    }

    char *comment_pos = find_comment_marker(line, flags);
    if (comment_pos) {
        *comment_pos = '\0';
    }

    char *p = line;
    
    if (flags & CSV_FLAG_TRIM_WHITESPACE) {
        while (ISSPACE(*p)) p++;
    }
    
    if (*p == '\0' || *p == '\n') {
        result->error_code = CSV_ERR_EMPTY_LINE;
        result->error_msg = "Empty line";
        return CSV_ERR_EMPTY_LINE;
    }

    int count = 0;
    char *start = p;
    int brace_depth = 0;
    bool in_quotes = false;
    
    while (*p) {
        if (flags & CSV_FLAG_ALLOW_QUOTES && *p == '"') {
            in_quotes = !in_quotes;
            p++;
            continue;
        }

        if (!in_quotes) {
            if (flags & CSV_FLAG_BRACE_AWARE && *p == '{') {
                brace_depth++;
            } else if (flags & CSV_FLAG_BRACE_AWARE && *p == '}') {
                if (brace_depth > 0) brace_depth--;
            } else if (*p == delimiter && brace_depth == 0) {
                *p = '\0';

                if (count >= max_fields) {
                    result->error_code = CSV_ERR_TOO_MANY_FIELDS;
                    result->error_msg = "Too many fields";
                    return CSV_ERR_TOO_MANY_FIELDS;
                }

                char *field = start;
                
                if (flags & CSV_FLAG_TRIM_WHITESPACE) {
                    field = trim_whitespace(field);
                }

                if ((flags & CSV_FLAG_EMPTY_AS_NULL) && *field == '\0') {
                    result->fields[count++] = NULL;
                } else {
                    result->fields[count++] = field;
                }

                p++;
                if (flags & CSV_FLAG_TRIM_WHITESPACE) {
                    while (ISSPACE(*p)) p++;
                }
                start = p;
                continue;
            }
        }
        p++;
    }

    if (start <= p) {
        if (count >= max_fields) {
            result->error_code = CSV_ERR_TOO_MANY_FIELDS;
            result->error_msg = "Too many fields";
            return CSV_ERR_TOO_MANY_FIELDS;
        }

        char *field = start;
        char *end = p - 1;

        while (end >= start && (*end == '\n' || *end == '\r' ||
               ((flags & CSV_FLAG_TRIM_WHITESPACE) && ISSPACE(*end)))) {
            *end-- = '\0';
        }
        
        if (flags & CSV_FLAG_TRIM_WHITESPACE) {
            field = trim_whitespace(field);
        }
        
        if ((flags & CSV_FLAG_EMPTY_AS_NULL) && *field == '\0') {
            result->fields[count] = NULL;
        } else {
            result->fields[count] = field;
        }
        count++;
    }

    if (flags & CSV_FLAG_BRACE_AWARE && brace_depth > 0) {
        result->error_code = CSV_ERR_UNCLOSED_BRACE;
        result->error_msg = "Unclosed { brace";
        return CSV_ERR_UNCLOSED_BRACE;
    }

    if (flags & CSV_FLAG_ALLOW_QUOTES && in_quotes) {
        result->error_code = CSV_ERR_UNCLOSED_QUOTE;
        result->error_msg = "Unclosed \" quote";
        return CSV_ERR_UNCLOSED_QUOTE;
    }

    if (count < max_fields) {
        result->field_count = count;
        result->error_code = CSV_ERR_TOO_FEW_FIELDS;
        result->error_msg = "Insufficient columns";
        return CSV_ERR_TOO_FEW_FIELDS;
    }

    result->field_count = count;
    result->error_code = CSV_OK;
    result->error_msg = "OK";

    return CSV_OK;
}

int csv_get_int_raw(const csv_result_t *res, const int idx, const int default_val) {
    if (!res || idx < 0 || idx >= res->field_count) return default_val;
    if (!res->fields[idx]) return default_val;

    char *endptr;
    const long val = strtol(res->fields[idx], &endptr, 10);

    if (*endptr != '\0' && !ISSPACE(*endptr)) return default_val;

    return (int)val;
}

const char* csv_get_str_raw(const csv_result_t* res, const int idx, const char* def) {
    if (!res || idx < 0 || idx >= res->field_count || !res->fields[idx]) {
        return def ? def : "";
    }
    return res->fields[idx];
}

static char* find_comment_marker(char *str, const uint flags) {
    bool in_quotes = false;
    char *p = str;

    while (*p) {
        if ((flags & CSV_FLAG_ALLOW_QUOTES) && *p == '"') {
            in_quotes = !in_quotes;
            p++;
            continue;
        }

        if (!in_quotes && p[0] == '/' && p[1] == '/') {
            return p;
        }
        p++;
    }
    return NULL;
}