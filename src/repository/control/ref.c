/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/ref.c
 *
 * PURPOSE:
 *   Validate Git reference names before they reach native process invocation.
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
#include "umicom/repository/ref.h"

#include <ctype.h>
#include <string.h>

/* Check that repository ref satisfies its contract before another service relies on it. */
int umi_repository_ref_is_valid(const char *text)
{
    size_t length;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return 0;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_REPOSITORY_CONTROL_REF_CAPACITY) return 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (text[0] == '/' || text[length - 1U] == '/' ||
        text[0] == '.' || text[length - 1U] == '.' ||
        strstr(text, "..") != NULL || strstr(text, "@{") != NULL ||
        strstr(text, "//") != NULL) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)text[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (iscntrl(ch) != 0 || ch == ' ' || ch == '~' || ch == '^' ||
            ch == ':' || ch == '?' || ch == '*' || ch == '[' ||
            ch == '\\') {
            return 0;
        }
    }
    return 1;
}

/*
 * Copy repository ref into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_repository_ref_copy(
    const char *text, char *out_ref, size_t capacity)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!umi_repository_ref_is_valid(text) || out_ref == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_ref, text, length + 1U);
    return UMI_STATUS_OK;
}
