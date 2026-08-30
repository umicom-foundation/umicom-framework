/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/pagination.h
 *
 * PURPOSE:
 *   Compute page counts and logical row ranges for paged enterprise providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_PAGINATION_H
#define UMICOM_UI_ENTERPRISE_PAGINATION_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntPagination { size_t page; size_t page_size; size_t total_rows; } UmiUiEntPagination;
size_t umi_ui_ent_pagination_page_count(const UmiUiEntPagination *p);
UmiUiEntSpan umi_ui_ent_pagination_span(const UmiUiEntPagination *p);

#endif
