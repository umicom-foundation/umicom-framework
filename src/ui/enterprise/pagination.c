/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/pagination.c
 *
 * PURPOSE:
 *   Implement safe page calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/pagination.h"
/*
 * Return the number of records represented by ui ent pagination page without changing
 * their state.
 */
size_t umi_ui_ent_pagination_page_count(const UmiUiEntPagination *p){/* Apply this branch only when its contract condition is satisfied. */ if(!p||p->page_size==0U)return 0U;return p->total_rows/p->page_size+(p->total_rows%p->page_size?1U:0U);}
/*
 * Provide the ui ent pagination span operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_pagination_span(const UmiUiEntPagination *p){size_t first,remaining;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||p->page_size==0U)return (UmiUiEntSpan){0U,0U};/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->page>SIZE_MAX/p->page_size)return (UmiUiEntSpan){p->total_rows,0U};first=p->page*p->page_size;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(first>=p->total_rows)return (UmiUiEntSpan){p->total_rows,0U};remaining=p->total_rows-first;return (UmiUiEntSpan){first,remaining<p->page_size?remaining:p->page_size};}
