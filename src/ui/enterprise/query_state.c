/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/query_state.c
 *
 * PURPOSE:
 *   Implement revisioned virtual query state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/query_state.h"
void umi_ui_ent_query_state_init(UmiUiEntQueryState *q,size_t size){if(q){q->page=0U;q->page_size=size?size:50U;q->sort_count=0U;q->filter_count=0U;q->revision=1U;}}
UmiStatus umi_ui_ent_query_state_set_page(UmiUiEntQueryState *q,size_t page){if(!q)return UMI_STATUS_INVALID_ARGUMENT;q->page=page;q->revision++;return UMI_STATUS_OK;}
void umi_ui_ent_query_state_set_rules(UmiUiEntQueryState *q,size_t s,size_t f){if(q){q->sort_count=s;q->filter_count=f;q->page=0U;q->revision++;}}
