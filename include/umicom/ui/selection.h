/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/selection.h
 *
 * PURPOSE:
 *   Define a revisioned multi-selection model shared by editors, trees, tables and
 *   other frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_SELECTION_H
#define UMICOM_UI_SELECTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_SELECTION_MAX 256U
/**
 * Represent the ui selection model data shared with callers of this public contract.
 */
typedef struct UmiUiSelectionModel UmiUiSelectionModel;
/**
 * Initialise ui selection from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_selection_create(UmiUiSelectionModel **out_selection);
/**
 * Release or reset state held by ui selection so the same storage can be reused safely.
 */
void umi_ui_selection_destroy(UmiUiSelectionModel *selection);
/**
 * Provide the ui selection set single operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_selection_set_single(UmiUiSelectionModel *selection, const char *item_id);
/**
 * Add ui selection only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_selection_add(UmiUiSelectionModel *selection, const char *item_id);
/**
 * Remove ui selection while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_selection_remove(UmiUiSelectionModel *selection, const char *item_id);
/**
 * Release or reset state held by ui selection so the same storage can be reused safely.
 */
void umi_ui_selection_clear(UmiUiSelectionModel *selection);
/**
 * Provide the ui selection contains operation used by this module and its client
 * applications.
 */
int umi_ui_selection_contains(const UmiUiSelectionModel *selection, const char *item_id);
/**
 * Return the number of records represented by ui selection without changing their state.
 */
size_t umi_ui_selection_count(const UmiUiSelectionModel *selection);
/**
 * Provide the ui selection revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_selection_revision(const UmiUiSelectionModel *selection);
/**
 * Find ui selection while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_selection_at(const UmiUiSelectionModel *selection, size_t index, char *out_id, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
