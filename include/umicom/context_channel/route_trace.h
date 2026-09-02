/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/route_trace.h
 *
 * PURPOSE:
 *   Explain context routing decisions and route hops.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ROUTE_TRACE_H
#define UMICOM_CONTEXT_CHANNEL_ROUTE_TRACE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_ROUTE_TRACE_MAX_ITEMS 256U
/**
 * Represent the context route trace data shared with callers of this public contract.
 */
typedef struct UmiContextRouteTrace {
    uint32_t structure_size;
    char trace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char target_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char route_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint32_t hop;
    UmiStatus status;
    uint64_t revision;
} UmiContextRouteTrace;
/**
 * Represent the context route trace store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextRouteTraceStore {
    UmiContextRouteTrace items[UMI_CONTEXT_ROUTE_TRACE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextRouteTraceStore;
/**
 * Initialise context route trace from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_route_trace_init(UmiContextRouteTrace *record);
/**
 * Check that context route trace satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_route_trace_validate(const UmiContextRouteTrace *record);
/**
 * Initialise context route trace store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_route_trace_store_init(UmiContextRouteTraceStore *store);
/**
 * Provide the context route trace store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_route_trace_store_put(UmiContextRouteTraceStore *store,const UmiContextRouteTrace *record);
/**
 * Remove context route trace store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_route_trace_store_remove(UmiContextRouteTraceStore *store,const char *identity);
/**
 * Find context route trace store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextRouteTrace *umi_context_route_trace_store_find(UmiContextRouteTraceStore *store,const char *identity);
/**
 * Provide the context route trace store find const operation used by this module and its
 * client applications.
 */
const UmiContextRouteTrace *umi_context_route_trace_store_find_const(const UmiContextRouteTraceStore *store,const char *identity);
/**
 * Return the number of records represented by context route trace store without changing
 * their state.
 */
size_t umi_context_route_trace_store_count(const UmiContextRouteTraceStore *store);
/**
 * Provide the context route trace store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_route_trace_store_snapshot(const UmiContextRouteTraceStore *store,UmiContextRouteTrace *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
