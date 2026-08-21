/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/route.h
 *
 * PURPOSE:
 *   Describe deterministic routing between named context channels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ROUTE_H
#define UMICOM_CONTEXT_CHANNEL_ROUTE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_ROUTE_MAX_ITEMS 128U
typedef struct UmiContextRoute {
    uint32_t structure_size;
    char route_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char target_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char required_schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool enabled;
    bool allow_self_route;
    uint32_t priority;
    uint64_t revision;
} UmiContextRoute;
typedef struct UmiContextRouteStore {
    UmiContextRoute items[UMI_CONTEXT_ROUTE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextRouteStore;
void umi_context_route_init(UmiContextRoute *record);
UmiStatus umi_context_route_validate(const UmiContextRoute *record);
void umi_context_route_store_init(UmiContextRouteStore *store);
UmiStatus umi_context_route_store_put(UmiContextRouteStore *store,const UmiContextRoute *record);
UmiStatus umi_context_route_store_remove(UmiContextRouteStore *store,const char *identity);
UmiContextRoute *umi_context_route_store_find(UmiContextRouteStore *store,const char *identity);
const UmiContextRoute *umi_context_route_store_find_const(const UmiContextRouteStore *store,const char *identity);
size_t umi_context_route_store_count(const UmiContextRouteStore *store);
UmiStatus umi_context_route_store_snapshot(const UmiContextRouteStore *store,UmiContextRoute *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
