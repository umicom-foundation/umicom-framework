/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_trace.h
 *
 * PURPOSE:
 *   Expose sized caller-owned trace state for bounded Umicom C previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_CVM_TRACE_H
#define UMICOM_TOOLCHAIN_CVM_TRACE_H
#include "umicom/toolchain/cvm_preview.h"
#include "umicom_c/trace.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Only entries below count are current. Reuse clears count, not old byte storage;
 * a consumer must never display stale entries after a shorter or failed compile. */
typedef struct UmiCvmPreviewTrace {
    size_t count;
    UmcTraceStep steps[UMC_PROGRAM_LIMIT];
} UmiCvmPreviewTrace;

/* All three objects are stable, caller-owned and disjoint from the source span.
 * Wrong sizes or NULL storage leave every object unchanged. Accepted storage
 * starts a new request: result is invalidated and trace.count becomes zero.
 * Runtime faults retain trace steps but never set result.completed. This adapter
 * uses Umicom C's executor; it does not implement another arithmetic engine. */
UmiStatus umi_cvm_preview_traced(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size,
    UmiCvmPreviewTrace *trace, size_t trace_size);

#ifdef __cplusplus
}
#endif
#endif
