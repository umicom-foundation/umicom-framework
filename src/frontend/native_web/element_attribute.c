/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/element_attribute.c
 *
 * PURPOSE:
 *   Manage bounded semantic element attributes with unique-name replacement semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/element_attribute.h"

#include <string.h>

const char *umi_native_web_element_attribute_get(const UmiNativeWebSemanticElement *element, const char *name)
{
    size_t i;
    if (element == NULL || name == NULL) return NULL;
    for (i = 0U; i < element->attribute_count; ++i) if (strcmp(element->attributes[i].name, name) == 0) return element->attributes[i].value;
    return NULL;
}

UmiStatus umi_native_web_element_attribute_set(UmiNativeWebSemanticElement *element, const char *name, const char *value)
{
    size_t i; UmiStatus status;
    if (element == NULL || name == NULL || value == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < element->attribute_count; ++i) if (strcmp(element->attributes[i].name, name) == 0) {
        status = umi_native_web_copy_text(element->attributes[i].value, sizeof(element->attributes[i].value), value);
        if (status == UMI_STATUS_OK) ++element->revision;
        return status;
    }
    if (element->attribute_count >= UMI_NATIVE_WEB_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_native_web_copy_text(element->attributes[element->attribute_count].name, sizeof(element->attributes[element->attribute_count].name), name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_native_web_copy_text(element->attributes[element->attribute_count].value, sizeof(element->attributes[element->attribute_count].value), value);
    if (status != UMI_STATUS_OK) return status;
    ++element->attribute_count; ++element->revision; return UMI_STATUS_OK;
}

UmiStatus umi_native_web_element_attribute_remove(UmiNativeWebSemanticElement *element, const char *name)
{
    size_t i;
    if (element == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < element->attribute_count; ++i) if (strcmp(element->attributes[i].name, name) == 0) {
        if (i + 1U < element->attribute_count) (void)memmove(&element->attributes[i], &element->attributes[i+1U], (element->attribute_count-i-1U)*sizeof(element->attributes[0]));
        --element->attribute_count; ++element->revision; return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

