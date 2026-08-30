/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/sha.c
 *
 * PURPOSE:
 *   Validate and normalize Git object identifiers without shell parsing.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/sha.h"

#include <ctype.h>
#include <string.h>

int umi_repository_sha_is_valid(const char *text)
{
    size_t length;
    size_t index;
    if (text == NULL) return 0;
    length = strlen(text);
    if (length != 40U && length != 64U) return 0;
    for (index = 0U; index < length; ++index) {
        if (isxdigit((unsigned char)text[index]) == 0) return 0;
    }
    return 1;
}

UmiStatus umi_repository_sha_normalize(
    const char *text, char *out_sha, size_t capacity)
{
    size_t length;
    size_t index;
    if (!umi_repository_sha_is_valid(text) || out_sha == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < length; ++index) {
        out_sha[index] = (char)tolower((unsigned char)text[index]);
    }
    out_sha[length] = '\0';
    return UMI_STATUS_OK;
}
