/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/semantic_element.h
 *
 * PURPOSE:
 *   Define keyed semantic browser elements independent of any browser or desktop toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_SEMANTIC_ELEMENT_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_SEMANTIC_ELEMENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

/**
 * Represent the native web element attribute data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebElementAttribute { char name[64]; char value[UMI_NATIVE_WEB_VALUE_CAPACITY]; } UmiNativeWebElementAttribute;
/**
 * Represent the native web semantic element data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebSemanticElement {
    char element_id[UMI_NATIVE_WEB_ID_CAPACITY];
    char parent_id[UMI_NATIVE_WEB_ID_CAPACITY];
    char tag[32];
    char role[64];
    char css_class[UMI_NATIVE_WEB_TEXT_CAPACITY];
    char text[UMI_NATIVE_WEB_VALUE_CAPACITY];
    UmiNativeWebElementAttribute attributes[UMI_NATIVE_WEB_MAX_ATTRIBUTES];
    size_t attribute_count;
    int32_t order;
    uint64_t revision;
} UmiNativeWebSemanticElement;
/* Initialise a keyed semantic element with an HTML tag. */
UmiStatus umi_native_web_semantic_element_init(UmiNativeWebSemanticElement *element, const char *element_id, const char *tag);
/* Validate stable element identity, tag and bounded attribute state. */
UmiStatus umi_native_web_semantic_element_validate(const UmiNativeWebSemanticElement *element);

#ifdef __cplusplus
}
#endif
#endif
