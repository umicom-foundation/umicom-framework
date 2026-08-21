/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/route_trace.h
 *
 * PURPOSE:
 *   Explain context routing decisions and route hops.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ROUTE_TRACE_H
#define UMICOM_CONTEXT_CHANNEL_ROUTE_TRACE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_ROUTE_TRACE_MAX_ITEMS 256U
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
typedef struct UmiContextRouteTraceStore {
    UmiContextRouteTrace items[UMI_CONTEXT_ROUTE_TRACE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextRouteTraceStore;
void umi_context_route_trace_init(UmiContextRouteTrace *record);
UmiStatus umi_context_route_trace_validate(const UmiContextRouteTrace *record);
void umi_context_route_trace_store_init(UmiContextRouteTraceStore *store);
UmiStatus umi_context_route_trace_store_put(UmiContextRouteTraceStore *store,const UmiContextRouteTrace *record);
UmiStatus umi_context_route_trace_store_remove(UmiContextRouteTraceStore *store,const char *identity);
UmiContextRouteTrace *umi_context_route_trace_store_find(UmiContextRouteTraceStore *store,const char *identity);
const UmiContextRouteTrace *umi_context_route_trace_store_find_const(const UmiContextRouteTraceStore *store,const char *identity);
size_t umi_context_route_trace_store_count(const UmiContextRouteTraceStore *store);
UmiStatus umi_context_route_trace_store_snapshot(const UmiContextRouteTraceStore *store,UmiContextRouteTrace *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
