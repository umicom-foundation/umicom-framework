/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/diagnostics.c
 *
 * PURPOSE:
 *   Implement bounded structured diagnostic retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/diagnostics.h"
#include <string.h>
/*
 * Initialise workbench context host diagnostics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_diagnostics_init(UmiWorkbenchContextHostDiagnostics *d)
{/* Apply this branch only when its contract condition is satisfied. */ if(d){memset(d,0,sizeof(*d));d->revision=1U;}}
/*
 * Add workbench context host diagnostics only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_context_host_diagnostics_add(
    UmiWorkbenchContextHostDiagnostics *d,const UmiWorkbenchContextHostDiagnostic *item)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!d||!item||item->diagnostic_id[0]=='\0'||item->message[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(d->count==UMI_WORKBENCH_CONTEXT_HOST_MAX_DIAGNOSTICS){
        memmove(&d->items[0],&d->items[1],(d->count-1U)*sizeof(d->items[0]));--d->count;++d->dropped_count;}
    d->items[d->count++]=*item;++d->revision;return UMI_STATUS_OK;
}
/*
 * Release or reset state held by workbench context host diagnostics so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_diagnostics_clear(UmiWorkbenchContextHostDiagnostics *d)
{/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d){memset(d->items,0,sizeof(d->items));d->count=0U;++d->revision;}}
