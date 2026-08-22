/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/diagnostics.c
 *
 * PURPOSE:
 *   Implement bounded structured diagnostic retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/diagnostics.h"
#include <string.h>
void umi_workbench_context_host_diagnostics_init(UmiWorkbenchContextHostDiagnostics *d)
{if(d){memset(d,0,sizeof(*d));d->revision=1U;}}
UmiStatus umi_workbench_context_host_diagnostics_add(
    UmiWorkbenchContextHostDiagnostics *d,const UmiWorkbenchContextHostDiagnostic *item)
{
    if(!d||!item||item->diagnostic_id[0]=='\0'||item->message[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(d->count==UMI_WORKBENCH_CONTEXT_HOST_MAX_DIAGNOSTICS){
        memmove(&d->items[0],&d->items[1],(d->count-1U)*sizeof(d->items[0]));--d->count;++d->dropped_count;}
    d->items[d->count++]=*item;++d->revision;return UMI_STATUS_OK;
}
void umi_workbench_context_host_diagnostics_clear(UmiWorkbenchContextHostDiagnostics *d)
{if(d){memset(d->items,0,sizeof(d->items));d->count=0U;++d->revision;}}
