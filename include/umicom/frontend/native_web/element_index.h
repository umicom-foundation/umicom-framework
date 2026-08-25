/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/element_index.h
 *
 * PURPOSE:
 *   Build and query a stable element-ID index for fast render/diff operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_INDEX_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_ELEMENT_INDEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/element_tree.h"

typedef struct UmiNativeWebElementIndexEntry { char element_id[UMI_NATIVE_WEB_ID_CAPACITY]; size_t index; } UmiNativeWebElementIndexEntry;
typedef struct UmiNativeWebElementIndex { UmiNativeWebElementIndexEntry entries[UMI_NATIVE_WEB_MAX_ELEMENTS]; size_t count; uint64_t source_revision; } UmiNativeWebElementIndex;
/* Rebuild a deterministic ID index from the current element tree. */
UmiStatus umi_native_web_element_index_build(UmiNativeWebElementIndex *index, const UmiNativeWebElementTree *tree);
/* Resolve an element ID to its source tree index. */
UmiStatus umi_native_web_element_index_find(const UmiNativeWebElementIndex *index, const char *element_id, size_t *out_index);

#ifdef __cplusplus
}
#endif
#endif
