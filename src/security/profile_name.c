/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/profile_name.c
 *
 * PURPOSE:
 *   Validate profile names through one portable policy shared by native and cross-target clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/profile_name.h"
#include <string.h>

/* This bounded scan belongs only to the string convenience API. Account
 * password handling retains its own limit in local_profile.c. Neither path
 * attempts to infer the readable size of a caller-owned pointer. */
static size_t ProfileLength(const char *text, size_t limit)
{
    size_t n = 0U;
    if (text == NULL) return limit;
    while (n < limit && text[n] != '\0') ++n;
    return n;
}
/* Form and protocol adapters can carry bounded byte spans. Centralising the
 * policy here avoids a temporary string copy and keeps every application on the
 * same naming rules. Read and validate completely before changing caller state. */
UmiStatus UmiLocalProfileNameBytes(const char *name, size_t length,
                                 char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    char value[UMI_LOCAL_PROFILE_NAME_CAPACITY] = {0};
    if (name == NULL || out == NULL || length < 3U || length >= sizeof(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (c >= 'A' && c <= 'Z') c = (unsigned char)(c + ('a' - 'A'));
        int letter = c >= 'a' && c <= 'z';
        if (!letter && (i == 0U || !((c >= '0' && c <= '9') || c == '-' || c == '_')))
            return UMI_STATUS_INVALID_ARGUMENT;
        value[i] = (char)c;
    }
    memcpy(out, value, sizeof(value));
    return UMI_STATUS_OK;
}

/* Keep the public string entry point while sharing its policy with byte spans.
 * Existing callers still provide a valid C string. Bounded readers use Bytes. */
UmiStatus UmiLocalProfileName(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    return UmiLocalProfileNameBytes(name,
        ProfileLength(name, UMI_LOCAL_PROFILE_NAME_CAPACITY), out);
}

