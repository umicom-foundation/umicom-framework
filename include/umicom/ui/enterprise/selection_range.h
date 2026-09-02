/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/selection_range.h
 *
 * PURPOSE:
 *   Represent and normalise rectangular cell-selection ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SELECTION_RANGE_H
#define UMICOM_UI_ENTERPRISE_SELECTION_RANGE_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent selection range data shared with callers of this public contract.
 */
typedef struct UmiUiEntSelectionRange { size_t row_first,row_last,column_first,column_last; } UmiUiEntSelectionRange;
/**
 * Provide the ui ent selection range normalize operation used by this module and its
 * client applications.
 */
UmiUiEntSelectionRange umi_ui_ent_selection_range_normalize(UmiUiEntSelectionRange value);
/**
 * Provide the ui ent selection range contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_selection_range_contains(UmiUiEntSelectionRange value,size_t row,size_t column);

#endif
