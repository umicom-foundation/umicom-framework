/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/trace.h
 *
 * PURPOSE:
 *   Define trace spans, parent-child correlation and bounded in-process trace
 *   retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_TRACE_H
#define UMICOM_DIAGNOSTICS_TRACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TRACE_NAME_CAPACITY 160U
#define UMI_TRACE_SPAN_MAX 4096U
/**
 * List the named trace span status values accepted by this public contract.
 */
typedef enum UmiTraceSpanStatus { UMI_TRACE_UNSET = 0, UMI_TRACE_OK = 1, UMI_TRACE_ERROR = 2 } UmiTraceSpanStatus;
/**
 * Represent the trace span data shared with callers of this public contract.
 */
typedef struct UmiTraceSpan {
    uint64_t trace_id;
    uint64_t span_id;
    uint64_t parent_span_id;
    char name[UMI_TRACE_NAME_CAPACITY];
    uint64_t started_at_ns;
    uint64_t ended_at_ns;
    UmiTraceSpanStatus status;
} UmiTraceSpan;
/**
 * Represent the trace store data shared with callers of this public contract.
 */
typedef struct UmiTraceStore UmiTraceStore;
/**
 * Initialise trace store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_trace_store_create(UmiTraceStore **out_store);
/**
 * Release or reset state held by trace store so the same storage can be reused safely.
 */
void umi_trace_store_destroy(UmiTraceStore *store);
/**
 * Provide the trace span begin operation used by this module and its client applications.
 */
UmiStatus umi_trace_span_begin(UmiTraceStore *store,
                               uint64_t trace_id,
                               uint64_t parent_span_id,
                               const char *name,
                               uint64_t now_ns,
                               uint64_t *out_span_id);
/**
 * Provide the trace span end operation used by this module and its client applications.
 */
UmiStatus umi_trace_span_end(UmiTraceStore *store,
                             uint64_t span_id,
                             UmiTraceSpanStatus status,
                             uint64_t now_ns);
/**
 * Return the number of records represented by trace store without changing their state.
 */
size_t umi_trace_store_count(const UmiTraceStore *store);
/**
 * Find trace store while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_trace_store_at(const UmiTraceStore *store, size_t index,
                             UmiTraceSpan *out_span);
#ifdef __cplusplus
}
#endif

#endif
