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

/* Check that repository sha satisfies its contract before another service relies on it. */
int umi_repository_sha_is_valid(const char *text)
{
    size_t length;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length != 40U && length != 64U) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (isxdigit((unsigned char)text[index]) == 0) return 0;
    }
    return 1;
}

/*
 * Provide the repository sha normalize operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_sha_normalize(
    const char *text, char *out_sha, size_t capacity)
{
    size_t length;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!umi_repository_sha_is_valid(text) || out_sha == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        out_sha[index] = (char)tolower((unsigned char)text[index]);
    }
    out_sha[length] = '\0';
    return UMI_STATUS_OK;
}
