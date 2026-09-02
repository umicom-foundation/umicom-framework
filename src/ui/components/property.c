/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/property.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/property.h"

#include <string.h>

/*
 * Copy ui component property into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_component_property_set(UmiUiComponentProperty *property,
                                        const char *key,
                                        const char *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || key == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t key_length = strlen(key);
    const size_t value_length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (key_length >= sizeof(property->key) ||
        value_length >= sizeof(property->value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(property->key, key, key_length + 1U);
    (void)memcpy(property->value, value, value_length + 1U);
    return UMI_STATUS_OK;
}
