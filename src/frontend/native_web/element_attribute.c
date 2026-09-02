/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/element_attribute.c
 *
 * PURPOSE:
 *   Manage bounded semantic element attributes with unique-name replacement semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/element_attribute.h"

#include <string.h>

/*
 * Provide the native web element attribute get operation used by this module and its
 * client applications.
 */
const char *umi_native_web_element_attribute_get(const UmiNativeWebSemanticElement *element, const char *name)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < element->attribute_count; ++i) /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(element->attributes[i].name, name) == 0) return element->attributes[i].value;
    return NULL;
}

/*
 * Copy native web element attribute into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_native_web_element_attribute_set(UmiNativeWebSemanticElement *element, const char *name, const char *value)
{
    size_t i; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL || name == NULL || value == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < element->attribute_count; ++i) /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(element->attributes[i].name, name) == 0) {
        status = umi_native_web_copy_text(element->attributes[i].value, sizeof(element->attributes[i].value), value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) ++element->revision;
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (element->attribute_count >= UMI_NATIVE_WEB_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_native_web_copy_text(element->attributes[element->attribute_count].name, sizeof(element->attributes[element->attribute_count].name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_native_web_copy_text(element->attributes[element->attribute_count].value, sizeof(element->attributes[element->attribute_count].value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ++element->attribute_count; ++element->revision; return UMI_STATUS_OK;
}

/*
 * Remove native web element attribute while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_native_web_element_attribute_remove(UmiNativeWebSemanticElement *element, const char *name)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < element->attribute_count; ++i) /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(element->attributes[i].name, name) == 0) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (i + 1U < element->attribute_count) (void)memmove(&element->attributes[i], &element->attributes[i+1U], (element->attribute_count-i-1U)*sizeof(element->attributes[0]));
        --element->attribute_count; ++element->revision; return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

