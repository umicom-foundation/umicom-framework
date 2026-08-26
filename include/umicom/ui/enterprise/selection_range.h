/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/selection_range.h
 *
 * PURPOSE:
 *   Represent and normalise rectangular cell-selection ranges.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SELECTION_RANGE_H
#define UMICOM_UI_ENTERPRISE_SELECTION_RANGE_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntSelectionRange { size_t row_first,row_last,column_first,column_last; } UmiUiEntSelectionRange;
UmiUiEntSelectionRange umi_ui_ent_selection_range_normalize(UmiUiEntSelectionRange value);
int umi_ui_ent_selection_range_contains(UmiUiEntSelectionRange value,size_t row,size_t column);

#endif
