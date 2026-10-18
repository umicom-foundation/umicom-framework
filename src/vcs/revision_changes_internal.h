/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/revision_changes_internal.h
 * Purpose: Share strict full-object identity validation for revision observations.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_REVISION_CHANGES_INTERNAL_H
#define UMICOM_VCS_REVISION_CHANGES_INTERNAL_H
#include <stdbool.h>
#include <stddef.h>
static inline bool UmiVcsRevisionHex(const char *text, size_t length)
{
    if (length != 40U && length != 64U) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
              (ch >= 'A' && ch <= 'F'))) return false;
    }
    return true;
}
#endif
