/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_trace.c
 *
 * PURPOSE:
 *   Record bounded, human-readable dependency-resolution decisions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_trace.h"


#include <string.h>
void umi_bootstrap_resolution_trace_init(UmiBootstrapResolutionTrace *trace) {
    if (trace != NULL) memset(trace, 0, sizeof(*trace));
}
UmiStatus umi_bootstrap_resolution_trace_append(UmiBootstrapResolutionTrace *trace,
                                                const char *step) {
    if (trace == NULL || step == NULL || step[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (trace->count >= UMI_BOOTSTRAP_MAX_TRACE) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_bootstrap_copy_text(trace->steps[trace->count],
        sizeof(trace->steps[trace->count]), step) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++trace->count;
    return UMI_STATUS_OK;
}
