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
/* Provide the span operation used by this module and its client applications. */
static UmiUiEntSpan span(size_t first,size_t visible,size_t over,size_t total){UmiUiEntSpan s={0U,0U};size_t start=first>over?first-over:0U;size_t end=first+visible;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(end<first)end=total;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(end>total)end=total;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(over>total-end)end=total;/* Use this fallback path when the earlier condition does not apply. */ else end+=over;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(start>total)start=total;s.first=start;s.count=end>=start?end-start:0U;return s;}
/*
 * Provide the ui ent viewport row span operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_viewport_row_span(const UmiUiEntViewport *v,size_t total){return v?span(v->first_row,v->visible_rows,v->row_overscan,total):(UmiUiEntSpan){0U,0U};}
/*
 * Provide the ui ent viewport column span operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_viewport_column_span(const UmiUiEntViewport *v,size_t total){return v?span(v->first_column,v->visible_columns,v->column_overscan,total):(UmiUiEntSpan){0U,0U};}
