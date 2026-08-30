/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/focus_model.h
 *
 * PURPOSE:
 *   Track the active virtual cell independently of widget focus objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FOCUS_MODEL_H
#define UMICOM_UI_ENTERPRISE_FOCUS_MODEL_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntFocusModel { size_t row,column; int active; uint64_t revision; } UmiUiEntFocusModel;
void umi_ui_ent_focus_model_clear(UmiUiEntFocusModel *m);
UmiStatus umi_ui_ent_focus_model_set(UmiUiEntFocusModel *m,size_t row,size_t column,size_t row_count,size_t column_count);

#endif
