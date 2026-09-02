/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/selection_model.h
 *
 * PURPOSE:
 *   Maintain multiple enterprise cell ranges with single/multi/range semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SELECTION_MODEL_H
#define UMICOM_UI_ENTERPRISE_SELECTION_MODEL_H

#include "umicom/ui/enterprise/selection_range.h"
/**
 * Represent the ui ent selection model data shared with callers of this public contract.
 */
typedef struct UmiUiEntSelectionModel { UmiUiEntSelectionMode mode; UmiUiEntSelectionRange ranges[UMI_UI_ENT_MAX_SELECTIONS]; size_t count; uint64_t revision; } UmiUiEntSelectionModel;
/**
 * Initialise ui ent selection model from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_selection_model_init(UmiUiEntSelectionModel *m,UmiUiEntSelectionMode mode);
/**
 * Add ui ent selection model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_selection_model_add(UmiUiEntSelectionModel *m,UmiUiEntSelectionRange range);
/**
 * Release or reset state held by ui ent selection model so the same storage can be reused
 * safely.
 */
void umi_ui_ent_selection_model_clear(UmiUiEntSelectionModel *m);
/**
 * Provide the ui ent selection model contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_selection_model_contains(const UmiUiEntSelectionModel *m,size_t row,size_t column);

#endif
