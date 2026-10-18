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
#include "umicom/document/text_encoding.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Provide the uri unreserved operation used by this module and its client applications. */
static int uri_unreserved(unsigned char value)
{
/* Locale-independent escaping keeps file identities stable across GTK locale initialisation. The former implementation is retained for review. */
#if 0
    return isalnum(value) || value == '-' || value == '.' ||
           value == '_' || value == '~' || value == '/' || value == ':';
#else
    /* URI syntax is ASCII even when the process locale classifies individual
     * UTF-8 bytes as alphabetic. Encode every non-ASCII byte consistently. */
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') ||
           (value >= '0' && value <= '9') || value == '-' || value == '.' ||
           value == '_' || value == '~' || value == '/' || value == ':';
#endif
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
/* Transactional UTF-8 encoding publishes a complete URI only after validation and capacity checks, preserving the caller buffer on failure. The former implementation is retained for review. */
#if 0
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
#else
UmiStatus umi_document_uri_from_path(const char *path, char *out_uri, size_t capacity)
{
    if (path == NULL || path[0] == '\0' || out_uri == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_document_utf8_validate((const unsigned char *)path, strlen(path), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(path, normalised, sizeof normalised);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_path_is_absolute(normalised)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t required = sizeof("file://");
    bool drivePrefix = false;
#ifdef _WIN32
    drivePrefix = normalised[0] != '\\' && normalised[0] != '/';
    if (drivePrefix) ++required;
#endif
    for (size_t index = 0U; normalised[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)normalised[index];
        if (value == '\\') value = '/';
        required += uri_unreserved(value) ? 1U : 3U;
    }
    if (required > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *candidate = malloc(required);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    static const char hex[] = "0123456789ABCDEF";
    memcpy(candidate, "file://", 7U);
    size_t used = 7U;
    if (drivePrefix) candidate[used++] = '/';
    for (size_t index = 0U; normalised[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)normalised[index];
        if (value == '\\') value = '/';
        if (uri_unreserved(value)) candidate[used++] = (char)value;
        else {
            candidate[used++] = '%';
            candidate[used++] = hex[value >> 4U];
            candidate[used++] = hex[value & 15U];
        }
    }
    candidate[used] = '\0';
    memcpy(out_uri, candidate, used + 1U);
    free(candidate);
    return UMI_STATUS_OK;
}
#endif
/*
 * Provide the document uri to path operation used by this module and its client
 * applications.
 */
/* Decode into owned temporary storage and reject embedded NUL, malformed UTF-8 and non-absolute paths before publishing a native filename. The former implementation is retained for review. */
#if 0
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
#else
UmiStatus umi_document_uri_to_path(const char *uri, char *out_path, size_t capacity)
{
    if (!umi_document_uri_is_file(uri) || out_path == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *input = uri + 7U;
    /* A Windows file URI adds one slash before its native drive prefix. */
    char decoded[UMI_PATH_CAPACITY + 1U], normalised[UMI_PATH_CAPACITY];
    size_t used = 0U;
    while (*input != '\0') {
        unsigned char value;
        if (*input == '%') {
            if (input[1] == '\0' || input[2] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
            int high = hex_value(input[1]), low = hex_value(input[2]);
            if (high < 0 || low < 0) return UMI_STATUS_INVALID_ARGUMENT;
            value = (unsigned char)((high << 4) | low);
            input += 3U;
            /* NUL would make later filesystem code see only a prefix. */
            if (value == 0U) return UMI_STATUS_INVALID_ARGUMENT;
        } else {
            value = (unsigned char)*input++;
            /* Query/fragment syntax is not a native filename. Real filename
             * punctuation and whitespace must be percent-encoded by callers. */
            if (value <= 32U || value == 127U || value == '?' || value == '#')
                return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (used + 1U >= sizeof decoded) return UMI_STATUS_CAPACITY_EXCEEDED;
        decoded[used++] = (char)value;
    }
    decoded[used] = '\0';
    if (used == 0U || !umi_document_utf8_validate((const unsigned char *)decoded, used, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *native = decoded;
#ifdef _WIN32
    /* Decode before recognising the drive so percent-encoded colons have the
     * same identity as the literal drive separator. Match ASCII letters only. */
    if (used >= 3U && native[0] == '/' && native[2] == ':' &&
        ((native[1] >= 'A' && native[1] <= 'Z') || (native[1] >= 'a' && native[1] <= 'z')))
        ++native;
#endif
    if (!umi_path_is_absolute(native)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_path_normalise(native, normalised, sizeof normalised);
    if (status != UMI_STATUS_OK) return status;
    size_t length = strlen(normalised);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_path, normalised, length + 1U);
    return UMI_STATUS_OK;
}
#endif
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

/* Reuse the file URI byte encoding policy, but a display name is one segment,
 * not a filesystem path. Measure and validate before publishing any output. */
UmiStatus umi_document_uri_from_untitled_name(const char *name, char *out_uri, size_t capacity)
{
    static const char prefix[] = "untitled:///";
    static const char hex[] = "0123456789ABCDEF";
    if (name == NULL || name[0] == '\0' || out_uri == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(name), required = sizeof(prefix);
    if (!umi_document_utf8_validate((const unsigned char *)name, length, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)name[i];
        size_t amount = uri_unreserved(ch) && ch != '/' && ch != ':' ? 1U : 3U;
        if (required > SIZE_MAX - amount) return UMI_STATUS_CAPACITY_EXCEEDED;
        required += amount;
    }
    if (required > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Input and output may overlap. Keep an owned candidate until encoding
     * finishes so in-place callers have the same contract as separate buffers. */
    char *candidate = malloc(required);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(candidate, prefix, sizeof(prefix) - 1U);
    size_t used = sizeof(prefix) - 1U;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)name[i];
        if (uri_unreserved(ch) && ch != '/' && ch != ':') candidate[used++] = (char)ch;
        else {
            candidate[used++] = '%';
            candidate[used++] = hex[ch >> 4U];
            candidate[used++] = hex[ch & 15U];
        }
    }
    candidate[used] = '\0';
    memcpy(out_uri, candidate, used + 1U);
    free(candidate);
    return UMI_STATUS_OK;
}
