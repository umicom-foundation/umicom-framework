/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/trace.h
 *
 * PURPOSE:
 *   Define trace spans, parent-child correlation and bounded in-process trace
 *   retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiTraceSpanStatus { UMI_TRACE_UNSET = 0, UMI_TRACE_OK = 1, UMI_TRACE_ERROR = 2 } UmiTraceSpanStatus;
typedef struct UmiTraceSpan {
    uint64_t trace_id;
    uint64_t span_id;
    uint64_t parent_span_id;
    char name[UMI_TRACE_NAME_CAPACITY];
    uint64_t started_at_ns;
    uint64_t ended_at_ns;
    UmiTraceSpanStatus status;
} UmiTraceSpan;
typedef struct UmiTraceStore UmiTraceStore;
UmiStatus umi_trace_store_create(UmiTraceStore **out_store);
void umi_trace_store_destroy(UmiTraceStore *store);
UmiStatus umi_trace_span_begin(UmiTraceStore *store,
                               uint64_t trace_id,
                               uint64_t parent_span_id,
                               const char *name,
                               uint64_t now_ns,
                               uint64_t *out_span_id);
UmiStatus umi_trace_span_end(UmiTraceStore *store,
                             uint64_t span_id,
                             UmiTraceSpanStatus status,
                             uint64_t now_ns);
size_t umi_trace_store_count(const UmiTraceStore *store);
UmiStatus umi_trace_store_at(const UmiTraceStore *store, size_t index,
                             UmiTraceSpan *out_span);
#ifdef __cplusplus
}
#endif

#endif
