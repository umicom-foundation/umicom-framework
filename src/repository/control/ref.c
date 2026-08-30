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

int umi_repository_ref_is_valid(const char *text)
{
    size_t length;
    size_t index;
    if (text == NULL || text[0] == '\0') return 0;
    length = strlen(text);
    if (length >= UMI_REPOSITORY_CONTROL_REF_CAPACITY) return 0;
    if (text[0] == '/' || text[length - 1U] == '/' ||
        text[0] == '.' || text[length - 1U] == '.' ||
        strstr(text, "..") != NULL || strstr(text, "@{") != NULL ||
        strstr(text, "//") != NULL) {
        return 0;
    }
    for (index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)text[index];
        if (iscntrl(ch) != 0 || ch == ' ' || ch == '~' || ch == '^' ||
            ch == ':' || ch == '?' || ch == '*' || ch == '[' ||
            ch == '\\') {
            return 0;
        }
    }
    return 1;
}

UmiStatus umi_repository_ref_copy(
    const char *text, char *out_ref, size_t capacity)
{
    size_t length;
    if (!umi_repository_ref_is_valid(text) || out_ref == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_ref, text, length + 1U);
    return UMI_STATUS_OK;
}
