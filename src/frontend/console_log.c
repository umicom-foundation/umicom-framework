/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/console_log.h"
#include <string.h>
UmiStatus umi_frontend_console_log_init(UmiFrontendConsoleLog *l){if(l==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(l,0,sizeof(*l));l->next_sequence=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_console_log_append(UmiFrontendConsoleLog *l,const UmiFrontendConsoleEntry *e){size_t p;if(l==NULL||e==NULL||e->message[0]=='\0'||e->level<UMI_FRONTEND_CONSOLE_DEBUG||e->level>UMI_FRONTEND_CONSOLE_ERROR)return UMI_STATUS_INVALID_ARGUMENT;if(l->count<UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES)p=l->count++;else{(void)memmove(&l->items[0],&l->items[1],(UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES-1U)*sizeof(l->items[0]));p=UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES-1U;}l->items[p]=*e;l->items[p].sequence=l->next_sequence++;return UMI_STATUS_OK;}
size_t umi_frontend_console_log_count_level(const UmiFrontendConsoleLog *l,UmiFrontendConsoleLevel m){size_t i,n=0U;if(l==NULL)return 0U;for(i=0U;i<l->count;++i)if(l->items[i].level>=m)n+=1U;return n;}
void umi_frontend_console_log_clear(UmiFrontendConsoleLog *l){if(l==NULL)return;l->count=0U;}
