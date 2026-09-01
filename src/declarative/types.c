/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/types.c
 *
 * PURPOSE:
 *   Implement small validation and text helpers used throughout declarative parsing and designer state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/types.h"

#include <ctype.h>
#include <string.h>

UmiDeclVersion umi_decl_version_current(void)
{
    UmiDeclVersion version = {1U, 0U, 0U};
    return version;
}

int umi_decl_version_equal(UmiDeclVersion left, UmiDeclVersion right)
{
    return left.major == right.major &&
           left.minor == right.minor &&
           left.patch == right.patch;
}

UmiStatus umi_decl_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

int umi_decl_id_is_valid(const char *identifier)
{
    size_t i;
    if (identifier == NULL || identifier[0] == '\0') return 0;
    for (i = 0U; identifier[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)identifier[i];
        if (!(isalnum(c) || c == '_' || c == '-' || c == '.')) return 0;
        if (i + 1U >= UMI_DECL_ID_CAPACITY) return 0;
    }
    return 1;
}

const char *umi_decl_value_kind_text(UmiDeclValueKind kind)
{
    switch (kind) {
        case UMI_DECL_VALUE_STRING: return "string";
        case UMI_DECL_VALUE_BOOLEAN: return "boolean";
        case UMI_DECL_VALUE_INTEGER: return "integer";
        case UMI_DECL_VALUE_REAL: return "real";
        default: return "unknown";
    }
}
