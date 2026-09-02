/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/query_history.c
 *
 * PURPOSE:
 *   Implement the query history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/query_history.h"
#include <string.h>
/*
 * Initialise database query history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_query_history_init(UmiDatabaseQueryHistory *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(h,0,sizeof(*h));h->next_sequence=1U;return UMI_STATUS_OK;}
/*
 * Add database query history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_database_query_history_append(UmiDatabaseQueryHistory *h,const UmiDatabaseQueryRecord *r){size_t p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||r==NULL||r->statement[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count<UMI_DATABASE_MAX_HISTORY)p=h->count++;/* Use this fallback path when the earlier condition does not apply. */ else{(void)memmove(&h->items[0],&h->items[1],(UMI_DATABASE_MAX_HISTORY-1U)*sizeof(h->items[0]));p=UMI_DATABASE_MAX_HISTORY-1U;}h->items[p]=*r;h->items[p].sequence=h->next_sequence++;return UMI_STATUS_OK;}
/*
 * Find database query history while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_database_query_history_at(const UmiDatabaseQueryHistory *h,size_t i,UmiDatabaseQueryRecord *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=h->count)return UMI_STATUS_NOT_FOUND;*out=h->items[i];return UMI_STATUS_OK;}
/*
 * Release or reset state held by database query history so the same storage can be reused
 * safely.
 */
void umi_database_query_history_clear(UmiDatabaseQueryHistory *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL)return;h->count=0U;}
