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

int csv_parse_line(char *line, csv_result_t *result, const int max_fields, const char delimiter, const uint32 flags) {
    if (!line || !result) return CSV_ERR_EMPTY_LINE;
    
    memset(result, 0, sizeof(csv_result_t));
    
    char *p = line;
    
    if (flags & CSV_FLAG_TRIM_WHITESPACE) {
        while (ISSPACE(*p)) p++;
    }
    
    if (*p == '\0' || *p == '\n') {
        result->error_code = CSV_ERR_EMPTY_LINE;
        result->error_msg = "Empty line";
        return CSV_ERR_EMPTY_LINE;
    }

    if ((flags & CSV_FLAG_COMMENT_SKIP) && p[0] == '/' && p[1] == '/') {
        result->error_code = CSV_ERR_EMPTY_LINE;
        result->error_msg = "Comment line";
        return CSV_ERR_EMPTY_LINE;
    }
    
    int count = 0;
    char *start = p;
    int brace_depth = 0;
    bool in_quotes = false;
    
    while (*p && count < max_fields) {
        if ((flags & CSV_FLAG_ALLOW_QUOTES) && *p == '"') {
            in_quotes = !in_quotes;
            p++;
            continue;
        }
        
        if (!in_quotes) {
            if ((flags & CSV_FLAG_BRACE_AWARE) && *p == '{') {
                brace_depth++;
            } else if ((flags & CSV_FLAG_BRACE_AWARE) && *p == '}') {
                if (brace_depth > 0) brace_depth--;
            } else if (*p == delimiter && brace_depth == 0) {
                *p = '\0';
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

    if (count < max_fields && start < p) {
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

    if (count < max_fields) {
        result->field_count = count;
        result->error_code = CSV_ERR_TOO_FEW_FIELDS;
        result->error_msg = "Insufficient columns";
        return CSV_ERR_TOO_FEW_FIELDS;
    }

    if ((flags & CSV_FLAG_BRACE_AWARE) && brace_depth > 0) {
        result->error_code = CSV_ERR_UNCLOSED_BRACE;
        result->error_msg = "Unclosed { brace";
        return CSV_ERR_UNCLOSED_BRACE;
    }

    if ((flags & CSV_FLAG_ALLOW_QUOTES) && in_quotes) {
        result->error_code = CSV_ERR_UNCLOSED_QUOTE;
        result->error_msg = "Unclosed \" quote";
        return CSV_ERR_UNCLOSED_QUOTE;
    }

    result->field_count = count;
    result->error_code = CSV_OK;
    result->error_msg = "OK";

    return CSV_OK;
}

int csv_get_int(const csv_result_t *res, const int idx, const int default_val) {
    if (!res || idx < 0 || idx >= res->field_count) return default_val;
    if (!res->fields[idx]) return default_val;
    
    char *endptr;
    const long val = strtol(res->fields[idx], &endptr, 10);

    if (*endptr != '\0' && !ISSPACE(*endptr)) return default_val;
    
    return (int)val;
}

const char* csv_get_str(const csv_result_t *res, const int idx) {
    if (!res || idx < 0 || idx >= res->field_count) return "";
    return res->fields[idx] ? res->fields[idx] : "";
}

bool csv_get_bool(const csv_result_t *res, const int idx, const bool default_val) {
    if (!res || idx < 0 || idx >= res->field_count) return default_val;
    if (!res->fields[idx]) return default_val;
    
    const char *s = res->fields[idx];
    if (*s == '1' ||
        strncasecmp(s, "true", 4) == 0 ||
        strncasecmp(s, "yes", 3) == 0 ||
        strncasecmp(s, "on", 2) == 0 ||
        *s == 'T' || *s == 'Y') {
        return true;
    }

    if (*s == '0' || 
        strncasecmp(s, "false", 5) == 0 ||
        strncasecmp(s, "no", 2) == 0 ||
        strncasecmp(s, "off", 3) == 0 ||
        *s == 'F' || *s == 'N') {
        return false;
    }

    return default_val;
}