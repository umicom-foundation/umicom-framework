/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/console_log.c
 *
 * PURPOSE:
 *   Implement the console log behavior for
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
#include "umicom/frontend/console_log.h"
#include <string.h>
/*
 * Initialise frontend console log from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_frontend_console_log_init(UmiFrontendConsoleLog *l){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(l,0,sizeof(*l));l->next_sequence=1U;return UMI_STATUS_OK;}
/* Add frontend console log only after its inputs and available capacity have been checked. */
UmiStatus umi_frontend_console_log_append(UmiFrontendConsoleLog *l,const UmiFrontendConsoleEntry *e){size_t p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||e==NULL||e->message[0]=='\0'||e->level<UMI_FRONTEND_CONSOLE_DEBUG||e->level>UMI_FRONTEND_CONSOLE_ERROR)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->count<UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES)p=l->count++;/* Use this fallback path when the earlier condition does not apply. */ else{(void)memmove(&l->items[0],&l->items[1],(UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES-1U)*sizeof(l->items[0]));p=UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES-1U;}l->items[p]=*e;l->items[p].sequence=l->next_sequence++;return UMI_STATUS_OK;}
/*
 * Provide the frontend console log count level operation used by this module and its
 * client applications.
 */
size_t umi_frontend_console_log_count_level(const UmiFrontendConsoleLog *l,UmiFrontendConsoleLevel m){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->items[i].level>=m)n+=1U;return n;}
/*
 * Release or reset state held by frontend console log so the same storage can be reused
 * safely.
 */
void umi_frontend_console_log_clear(UmiFrontendConsoleLog *l){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return;l->count=0U;}
