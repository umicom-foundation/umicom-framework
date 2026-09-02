/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/uri.c
 *
 * PURPOSE:
 *   Implement percent-safe file URI conversion for Windows and POSIX paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/uri.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Provide the uri unreserved operation used by this module and its client applications. */
static int uri_unreserved(unsigned char value)
{
    return isalnum(value) || value == '-' || value == '.' ||
           value == '_' || value == '~' || value == '/' || value == ':';
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char value)
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
 * Provide the document uri is file operation used by this module and its client
 * applications.
 */
int umi_document_uri_is_file(const char *uri)
{
    return uri != NULL && strncmp(uri, "file://", 7U) == 0;
}

/*
 * Provide the document uri from path operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_from_path(const char *path,
                                     char *out_uri,
                                     size_t capacity)
{
    char normalised[UMI_PATH_CAPACITY];
    size_t input;
    size_t output = 0U;
    static const char HEX[] = "0123456789ABCDEF";
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_uri == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(path, normalised, sizeof(normalised));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_path_is_absolute(normalised)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity < 8U) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_uri, "file://", 7U);
    output = 7U;
#ifdef _WIN32
    /* Apply this branch only when its contract condition is satisfied. */
    if (normalised[0] != '\\' && normalised[0] != '/') out_uri[output++] = '/';
#endif
    /* Visit each bounded item once so every record receives the same rule. */
    for (input = 0U; normalised[input] != '\0'; ++input) {
        unsigned char value = (unsigned char)normalised[input];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') value = '/';
        /* Apply this branch only when its contract condition is satisfied. */
        if (uri_unreserved(value)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (output + 2U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_uri[output++] = (char)value;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (output + 4U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_uri[output++] = '%';
            out_uri[output++] = HEX[value >> 4U];
            out_uri[output++] = HEX[value & 0x0FU];
        }
    }
    out_uri[output] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the document uri to path operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_to_path(const char *uri,
                                   char *out_path,
                                   size_t capacity)
{
    const char *input;
    size_t output = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!umi_document_uri_is_file(uri) || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    input = uri + 7U;
#ifdef _WIN32
    /* Apply this branch only when its contract condition is satisfied. */
    if (input[0] == '/' && isalpha((unsigned char)input[1]) && input[2] == ':') input += 1U;
#endif
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*input != '\0') {
        unsigned char value;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*input == '%') {
            int high;
            int low;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (input[1] == '\0' || input[2] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
            high = hex_value(input[1]);
            low = hex_value(input[2]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_INVALID_ARGUMENT;
            value = (unsigned char)((high << 4) | low);
            input += 3U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            value = (unsigned char)*input++;
        }
#ifdef _WIN32
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '/') value = '\\';
#endif
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output + 2U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_path[output++] = (char)value;
    }
    out_path[output] = '\0';
    return umi_path_normalise(out_path, out_path, capacity);
}

/*
 * Provide the document uri normalise operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_uri_normalise(const char *uri,
                                     char *out_uri,
                                     size_t capacity)
{
    char path[UMI_PATH_CAPACITY];
    UmiStatus status = umi_document_uri_to_path(uri, path, sizeof(path));
    return status == UMI_STATUS_OK
        ? umi_document_uri_from_path(path, out_uri, capacity) : status;
}
