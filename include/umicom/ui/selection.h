/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/selection.h
 *
 * PURPOSE:
 *   Define a revisioned multi-selection model shared by editors, trees, tables and
 *   other frontend adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiUiSelectionModel UmiUiSelectionModel;
UmiStatus umi_ui_selection_create(UmiUiSelectionModel **out_selection);
void umi_ui_selection_destroy(UmiUiSelectionModel *selection);
UmiStatus umi_ui_selection_set_single(UmiUiSelectionModel *selection, const char *item_id);
UmiStatus umi_ui_selection_add(UmiUiSelectionModel *selection, const char *item_id);
UmiStatus umi_ui_selection_remove(UmiUiSelectionModel *selection, const char *item_id);
void umi_ui_selection_clear(UmiUiSelectionModel *selection);
int umi_ui_selection_contains(const UmiUiSelectionModel *selection, const char *item_id);
size_t umi_ui_selection_count(const UmiUiSelectionModel *selection);
uint64_t umi_ui_selection_revision(const UmiUiSelectionModel *selection);
UmiStatus umi_ui_selection_at(const UmiUiSelectionModel *selection, size_t index, char *out_id, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
