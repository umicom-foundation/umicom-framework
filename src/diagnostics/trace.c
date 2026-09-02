/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/trace.c
 *
 * PURPOSE:
 *   Implement chronological trace-span storage and deterministic span
 *   identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/trace.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "diagnostic_lock.h"
struct UmiTraceStore { UmiTraceSpan spans[UMI_TRACE_SPAN_MAX]; size_t count; size_t head; uint64_t next_span; UmiDiagnosticLock lock; };
/* Find span while leaving the underlying catalogue or model owned by this module. */
static size_t span_find(const UmiTraceStore *store, uint64_t span_id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < store->count; ++i) { size_t at = (store->head + i) % UMI_TRACE_SPAN_MAX; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (store->spans[at].span_id == span_id) return at; } return SIZE_MAX; }
/*
 * Initialise trace store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_trace_store_create(UmiTraceStore **out_store)
{
    UmiTraceStore *store;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL; store = (UmiTraceStore *)calloc(1U, sizeof(*store)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&store->lock);
    store->next_span = 1U; *out_store = store; return UMI_STATUS_OK;
}
/* Release or reset state held by trace store so the same storage can be reused safely. */
void umi_trace_store_destroy(UmiTraceStore *store)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store != NULL) { free(store); } }
/* Provide the trace span begin operation used by this module and its client applications. */
UmiStatus umi_trace_span_begin(UmiTraceStore *store, uint64_t trace_id, uint64_t parent_span_id, const char *name, uint64_t now_ns, uint64_t *out_span_id)
{
    size_t index; UmiTraceSpan *span;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || name == NULL || name[0] == '\0' || out_span_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(name) >= UMI_TRACE_NAME_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_diagnostic_lock_acquire(&store->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count < UMI_TRACE_SPAN_MAX) { index = (store->head + store->count) % UMI_TRACE_SPAN_MAX; ++store->count; }
    /* Use this fallback path when the earlier condition does not apply. */
    else { index = store->head; store->head = (store->head + 1U) % UMI_TRACE_SPAN_MAX; }
    span = &store->spans[index]; (void)memset(span, 0, sizeof(*span)); span->span_id = store->next_span++; span->trace_id = trace_id != 0U ? trace_id : span->span_id; span->parent_span_id = parent_span_id;
    span->started_at_ns = now_ns; (void)snprintf(span->name, sizeof(span->name), "%s", name); *out_span_id = span->span_id;
    umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_OK;
}
/* Provide the trace span end operation used by this module and its client applications. */
UmiStatus umi_trace_span_end(UmiTraceStore *store, uint64_t span_id, UmiTraceSpanStatus span_status, uint64_t now_ns)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || span_id == 0U || span_status < UMI_TRACE_UNSET || span_status > UMI_TRACE_ERROR) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&store->lock);
    index = span_find(store, span_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_NOT_FOUND; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->spans[index].ended_at_ns != 0U) { umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_INVALID_STATE; }
    store->spans[index].ended_at_ns = now_ns; store->spans[index].status = span_status;
    umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_OK;
}
/* Return the number of records represented by trace store without changing their state. */
size_t umi_trace_store_count(const UmiTraceStore *store)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store != NULL) { umi_diagnostic_lock_acquire(&store->lock); n = store->count; umi_diagnostic_lock_release(&store->lock); } return n; }
/* Find trace store while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_trace_store_at(const UmiTraceStore *store, size_t index, UmiTraceSpan *out_span)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_span == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&store->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= store->count) { umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_NOT_FOUND; }
    *out_span = store->spans[(store->head + index) % UMI_TRACE_SPAN_MAX]; umi_diagnostic_lock_release(&store->lock); return UMI_STATUS_OK;
}
