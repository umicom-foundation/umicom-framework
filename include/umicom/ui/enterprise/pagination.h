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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent pagination data shared with callers of this public contract.
 */
typedef struct UmiUiEntPagination { size_t page; size_t page_size; size_t total_rows; } UmiUiEntPagination;
/**
 * Return the number of records represented by ui ent pagination page without changing
 * their state.
 */
size_t umi_ui_ent_pagination_page_count(const UmiUiEntPagination *p);
/**
 * Provide the ui ent pagination span operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_pagination_span(const UmiUiEntPagination *p);


#ifdef __cplusplus
}
#endif
#endif
