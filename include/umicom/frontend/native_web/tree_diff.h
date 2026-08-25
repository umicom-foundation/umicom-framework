/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/tree_diff.h
 *
 * PURPOSE:
 *   Compute deterministic keyed element-tree differences as browser patch operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TREE_DIFF_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_TREE_DIFF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/element_tree.h"
#include "umicom/frontend/native_web/patch_batch.h"
/* Diff keyed element trees into a deterministic set of insert/remove/text/attribute patches. */
UmiStatus umi_native_web_tree_diff(const UmiNativeWebElementTree *previous, const UmiNativeWebElementTree *current, UmiNativeWebPatchBatch *out_batch);

#ifdef __cplusplus
}
#endif
#endif
