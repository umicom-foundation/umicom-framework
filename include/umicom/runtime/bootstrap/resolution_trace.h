/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_trace.h
 *
 * PURPOSE:
 *   Record bounded, human-readable dependency-resolution decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_TRACE_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_TRACE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap resolution trace from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_trace_init(UmiBootstrapResolutionTrace *trace);
/**
 * Add bootstrap resolution trace only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_resolution_trace_append(UmiBootstrapResolutionTrace *trace,
                                                const char *step);

#ifdef __cplusplus
}
#endif

#endif
