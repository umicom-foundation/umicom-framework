/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/selection_model.h
 *
 * PURPOSE:
 *   Maintain multiple enterprise cell ranges with single/multi/range semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SELECTION_MODEL_H
#define UMICOM_UI_ENTERPRISE_SELECTION_MODEL_H

#include "umicom/ui/enterprise/selection_range.h"
typedef struct UmiUiEntSelectionModel { UmiUiEntSelectionMode mode; UmiUiEntSelectionRange ranges[UMI_UI_ENT_MAX_SELECTIONS]; size_t count; uint64_t revision; } UmiUiEntSelectionModel;
void umi_ui_ent_selection_model_init(UmiUiEntSelectionModel *m,UmiUiEntSelectionMode mode);
UmiStatus umi_ui_ent_selection_model_add(UmiUiEntSelectionModel *m,UmiUiEntSelectionRange range);
void umi_ui_ent_selection_model_clear(UmiUiEntSelectionModel *m);
int umi_ui_ent_selection_model_contains(const UmiUiEntSelectionModel *m,size_t row,size_t column);

#endif
