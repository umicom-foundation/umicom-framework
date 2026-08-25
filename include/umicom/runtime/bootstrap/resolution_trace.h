/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_trace.h
 *
 * PURPOSE:
 *   Record bounded, human-readable dependency-resolution decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_TRACE_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_TRACE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_resolution_trace_init(UmiBootstrapResolutionTrace *trace);
UmiStatus umi_bootstrap_resolution_trace_append(UmiBootstrapResolutionTrace *trace,
                                                const char *step);

#ifdef __cplusplus
}
#endif

#endif
