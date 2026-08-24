/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/uri.c
 *
 * PURPOSE:
 *   Implement minimal local file URI decoding required for cross-navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/uri.h"

#include <ctype.h>
#include <string.h>

static int hex_digit(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

UmiStatus umi_ide_uri_to_local_path(
    const char *uri,
    char *out_path,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;

    if (uri == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = uri;

    if (strncmp(cursor, "file://", 7U) == 0) {
        cursor += 7U;

#ifdef _WIN32
        if (cursor[0] == '/' &&
            isalpha((unsigned char)cursor[1]) &&
            cursor[2] == ':') {
            cursor += 1U;
        }
#endif
    } else if (strstr(cursor, "://") != NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    while (*cursor != '\0') {
        unsigned char value;

        if (cursor[0] == '%' &&
            cursor[1] != '\0' &&
            cursor[2] != '\0') {
            const int high = hex_digit(cursor[1]);
            const int low = hex_digit(cursor[2]);

            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;

            value = (unsigned char)(((unsigned)high << 4U) | (unsigned)low);
            cursor += 3U;
        } else {
            value = (unsigned char)*cursor++;
        }

        if (used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

#ifdef _WIN32
        out_path[used++] =
            value == (unsigned char)'/'
                ? '\\'
                : (char)value;
#else
        out_path[used++] = (char)value;
#endif
    }

    out_path[used] = '\0';
    return UMI_STATUS_OK;
}
