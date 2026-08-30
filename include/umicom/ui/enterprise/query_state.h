/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/query_state.h
 *
 * PURPOSE:
 *   Track paging, sorting and filtering revisions for a virtual enterprise query.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_QUERY_STATE_H
#define UMICOM_UI_ENTERPRISE_QUERY_STATE_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntQueryState { size_t page; size_t page_size; size_t sort_count; size_t filter_count; uint64_t revision; } UmiUiEntQueryState;
void umi_ui_ent_query_state_init(UmiUiEntQueryState *q,size_t page_size);
UmiStatus umi_ui_ent_query_state_set_page(UmiUiEntQueryState *q,size_t page);
void umi_ui_ent_query_state_set_rules(UmiUiEntQueryState *q,size_t sort_count,size_t filter_count);

#endif
