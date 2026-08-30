/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/element_tree.h
 *
 * PURPOSE:
 *   Own a bounded server-side browser element tree with parent validation and deterministic order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_TREE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_TREE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/semantic_element.h"

typedef struct UmiNativeWebElementTree { UmiNativeWebSemanticElement elements[UMI_NATIVE_WEB_MAX_ELEMENTS]; size_t count; uint64_t revision; } UmiNativeWebElementTree;
/* Initialise an empty server-side semantic element tree. */
void umi_native_web_element_tree_init(UmiNativeWebElementTree *tree);
/* Insert or replace an element while enforcing parent/key consistency. */
UmiStatus umi_native_web_element_tree_upsert(UmiNativeWebElementTree *tree, const UmiNativeWebSemanticElement *element);
/* Remove an element only when no remaining element names it as parent. */
UmiStatus umi_native_web_element_tree_remove(UmiNativeWebElementTree *tree, const char *element_id);
/* Find an element by stable ID. */
const UmiNativeWebSemanticElement *umi_native_web_element_tree_find(const UmiNativeWebElementTree *tree, const char *element_id);

#ifdef __cplusplus
}
#endif
#endif
