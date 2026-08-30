/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/viewport.c
 *
 * PURPOSE:
 *   Compute clamped overscanned viewport spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/viewport.h"
static UmiUiEntSpan span(size_t first,size_t visible,size_t over,size_t total){UmiUiEntSpan s={0U,0U};size_t start=first>over?first-over:0U;size_t end=first+visible;if(end<first)end=total;if(end>total)end=total;if(over>total-end)end=total;else end+=over;if(start>total)start=total;s.first=start;s.count=end>=start?end-start:0U;return s;}
UmiUiEntSpan umi_ui_ent_viewport_row_span(const UmiUiEntViewport *v,size_t total){return v?span(v->first_row,v->visible_rows,v->row_overscan,total):(UmiUiEntSpan){0U,0U};}
UmiUiEntSpan umi_ui_ent_viewport_column_span(const UmiUiEntViewport *v,size_t total){return v?span(v->first_column,v->visible_columns,v->column_overscan,total):(UmiUiEntSpan){0U,0U};}
