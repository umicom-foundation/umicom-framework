/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/uri.c
 *
 * PURPOSE:
 *   Implement minimal local file URI decoding required for cross-navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/uri.h"

#include <ctype.h>
#include <string.h>

/* Provide the hex digit operation used by this module and its client applications. */
static int hex_digit(char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return value - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

/*
 * Provide the ide uri to local path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_uri_to_local_path(
    const char *uri,
    char *out_path,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (uri == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = uri;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(cursor, "file://", 7U) == 0) {
        cursor += 7U;

#ifdef _WIN32
        /* Apply this branch only when its contract condition is satisfied. */
        if (cursor[0] == '/' &&
            isalpha((unsigned char)cursor[1]) &&
            cursor[2] == ':') {
            cursor += 1U;
        }
#endif
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(cursor, "://") != NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        unsigned char value;

        /* Apply this branch only when its contract condition is satisfied. */
        if (cursor[0] == '%' &&
            cursor[1] != '\0' &&
            cursor[2] != '\0') {
            const int high = hex_digit(cursor[1]);
            const int low = hex_digit(cursor[2]);

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;

            value = (unsigned char)(((unsigned)high << 4U) | (unsigned)low);
            cursor += 3U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            value = (unsigned char)*cursor++;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
