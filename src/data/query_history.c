/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/query_history.h"
#include <string.h>
UmiStatus umi_database_query_history_init(UmiDatabaseQueryHistory *h){if(h==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(h,0,sizeof(*h));h->next_sequence=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_query_history_append(UmiDatabaseQueryHistory *h,const UmiDatabaseQueryRecord *r){size_t p;if(h==NULL||r==NULL||r->statement[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(h->count<UMI_DATABASE_MAX_HISTORY)p=h->count++;else{(void)memmove(&h->items[0],&h->items[1],(UMI_DATABASE_MAX_HISTORY-1U)*sizeof(h->items[0]));p=UMI_DATABASE_MAX_HISTORY-1U;}h->items[p]=*r;h->items[p].sequence=h->next_sequence++;return UMI_STATUS_OK;}
UmiStatus umi_database_query_history_at(const UmiDatabaseQueryHistory *h,size_t i,UmiDatabaseQueryRecord *out){if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=h->count)return UMI_STATUS_NOT_FOUND;*out=h->items[i];return UMI_STATUS_OK;}
void umi_database_query_history_clear(UmiDatabaseQueryHistory *h){if(h==NULL)return;h->count=0U;}
