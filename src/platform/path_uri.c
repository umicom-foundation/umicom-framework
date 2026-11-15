/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/path_uri.c
 *
 * PURPOSE:
 *   Share transactional native-path URI encoding with headless reports and document clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path_uri.h"
#include "umicom/platform/path.h"
#include "umicom/base/utf8.h"
#include <stdbool.h>
#include <string.h>

/* This is the former Document encoder's ASCII policy. URI bytes do not change
 * when an application initializes a different display locale. */
static bool path_uri_unreserved(unsigned char byte, bool absolute)
{
    return (byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z') ||
        (byte >= '0' && byte <= '9') || byte == '-' || byte == '.' ||
        byte == '_' || byte == '~' || byte == '/' || (absolute && byte == ':');
}

static UmiStatus path_uri_encode(const char *path, bool require_absolute,
    char *out_uri, size_t capacity)
{
    if (path == NULL || path[0] == '\0' || out_uri == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiUtf8Validate((const unsigned char *)path, strlen(path), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(path, normalised, sizeof normalised);
    if (status != UMI_STATUS_OK) return status;
    bool absolute = umi_path_is_absolute(normalised) != 0;
    if (require_absolute && !absolute) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (!absolute && (normalised[0] == '/' || normalised[0] == '\\' ||
        (normalised[0] != '\0' && normalised[1] == ':')))
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    /* The normalized input bounds this stack buffer. Build privately so even
     * a caller using one buffer for input and output sees no partial URI. */
    char candidate[UMI_PATH_CAPACITY * 3U + 9U];
    static const char hex[] = "0123456789ABCDEF";
    size_t used = 0U;
    if (absolute) {
        memcpy(candidate, "file://", 7U);
        used = 7U;
#ifdef _WIN32
        if (normalised[0] != '\\' && normalised[0] != '/') candidate[used++] = '/';
#endif
    }
    for (size_t index = 0U; normalised[index] != '\0'; ++index) {
        unsigned char byte = (unsigned char)normalised[index];
        if (byte == '\\') byte = '/';
        if (path_uri_unreserved(byte, absolute)) candidate[used++] = (char)byte;
        else {
            candidate[used++] = '%';
            candidate[used++] = hex[byte >> 4U];
            candidate[used++] = hex[byte & 15U];
        }
    }
    candidate[used] = '\0';
    if (used >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_uri, candidate, used + 1U);
    return UMI_STATUS_OK;
}

UmiStatus UmiFileUriFromPath(const char *path, char *out_uri, size_t capacity)
{
    return path_uri_encode(path, true, out_uri, capacity);
}

UmiStatus UmiPathUriReference(const char *path, char *out_uri, size_t capacity)
{
    return path_uri_encode(path, false, out_uri, capacity);
}
