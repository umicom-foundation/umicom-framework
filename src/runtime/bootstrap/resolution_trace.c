/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_trace.c
 *
 * PURPOSE:
 *   Implement the resolution trace behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_trace.c
 *
 * PURPOSE:
 *   Record bounded, human-readable dependency-resolution decisions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_trace.h"


#include <string.h>
/*
 * Initialise bootstrap resolution trace from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_trace_init(UmiBootstrapResolutionTrace *trace) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trace != NULL) memset(trace, 0, sizeof(*trace));
}
/*
 * Add bootstrap resolution trace only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_resolution_trace_append(UmiBootstrapResolutionTrace *trace,
                                                const char *step) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trace == NULL || step == NULL || step[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (trace->count >= UMI_BOOTSTRAP_MAX_TRACE) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_bootstrap_copy_text(trace->steps[trace->count],
        sizeof(trace->steps[trace->count]), step) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++trace->count;
    return UMI_STATUS_OK;
}
