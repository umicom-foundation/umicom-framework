/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/element_attribute.h
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

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_ATTRIBUTE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_ATTRIBUTE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/semantic_element.h"
/* Set or replace one attribute by name. */
UmiStatus umi_native_web_element_attribute_set(UmiNativeWebSemanticElement *element, const char *name, const char *value);
/* Remove one attribute if present. */
UmiStatus umi_native_web_element_attribute_remove(UmiNativeWebSemanticElement *element, const char *name);
/* Find one attribute by exact semantic name. */
const char *umi_native_web_element_attribute_get(const UmiNativeWebSemanticElement *element, const char *name);

#ifdef __cplusplus
}
#endif
#endif
