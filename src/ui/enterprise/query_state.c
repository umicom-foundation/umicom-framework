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
/*
 * Initialise ui ent query state from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_query_state_init(UmiUiEntQueryState *q,size_t size){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q){q->page=0U;q->page_size=size?size:50U;q->sort_count=0U;q->filter_count=0U;q->revision=1U;}}
/*
 * Provide the ui ent query state set page operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_query_state_set_page(UmiUiEntQueryState *q,size_t page){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!q)return UMI_STATUS_INVALID_ARGUMENT;q->page=page;q->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent query state set rules operation used by this module and its client
 * applications.
 */
void umi_ui_ent_query_state_set_rules(UmiUiEntQueryState *q,size_t s,size_t f){/* Apply this branch only when its contract condition is satisfied. */ if(q){q->sort_count=s;q->filter_count=f;q->page=0U;q->revision++;}}
