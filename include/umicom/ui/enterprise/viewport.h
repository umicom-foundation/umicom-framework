/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/viewport.h
 *
 * PURPOSE:
 *   Describe visible rows/columns and overscan for a virtualised view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_VIEWPORT_H
#define UMICOM_UI_ENTERPRISE_VIEWPORT_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntViewport { size_t first_row; size_t visible_rows; size_t first_column; size_t visible_columns; size_t row_overscan; size_t column_overscan; } UmiUiEntViewport;
UmiUiEntSpan umi_ui_ent_viewport_row_span(const UmiUiEntViewport *v,size_t total_rows);
UmiUiEntSpan umi_ui_ent_viewport_column_span(const UmiUiEntViewport *v,size_t total_columns);

#endif
