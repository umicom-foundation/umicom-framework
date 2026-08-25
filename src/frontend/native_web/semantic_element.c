/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/semantic_element.c
 *
 * PURPOSE:
 *   Define keyed semantic browser elements independent of any browser or desktop toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/semantic_element.h"

#include <string.h>

UmiStatus umi_native_web_semantic_element_init(UmiNativeWebSemanticElement *element, const char *element_id, const char *tag)
{
    UmiStatus status;
    if (element == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(element, 0, sizeof(*element));
    status = umi_native_web_copy_text(element->element_id, sizeof(element->element_id), element_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_native_web_copy_text(element->tag, sizeof(element->tag), tag);
    if (status != UMI_STATUS_OK) return status;
    element->revision = 1U;
    return umi_native_web_semantic_element_validate(element);
}

UmiStatus umi_native_web_semantic_element_validate(const UmiNativeWebSemanticElement *element)
{
    if (element == NULL || element->element_id[0] == '\0' || element->tag[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (element->attribute_count > UMI_NATIVE_WEB_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

