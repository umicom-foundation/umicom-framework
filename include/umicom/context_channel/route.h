/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/route.h
 *
 * PURPOSE:
 *   Describe deterministic routing between named context channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ROUTE_H
#define UMICOM_CONTEXT_CHANNEL_ROUTE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_ROUTE_MAX_ITEMS 128U
/**
 * Represent the context route data shared with callers of this public contract.
 */
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
/**
 * Represent the context route store data shared with callers of this public contract.
 */
typedef struct UmiContextRouteStore {
    UmiContextRoute items[UMI_CONTEXT_ROUTE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextRouteStore;
/**
 * Initialise context route from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_route_init(UmiContextRoute *record);
/**
 * Check that context route satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_route_validate(const UmiContextRoute *record);
/**
 * Initialise context route store from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_route_store_init(UmiContextRouteStore *store);
/**
 * Provide the context route store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_route_store_put(UmiContextRouteStore *store,const UmiContextRoute *record);
/**
 * Remove context route store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_route_store_remove(UmiContextRouteStore *store,const char *identity);
/**
 * Find context route store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextRoute *umi_context_route_store_find(UmiContextRouteStore *store,const char *identity);
/**
 * Provide the context route store find const operation used by this module and its client
 * applications.
 */
const UmiContextRoute *umi_context_route_store_find_const(const UmiContextRouteStore *store,const char *identity);
/**
 * Return the number of records represented by context route store without changing their
 * state.
 */
size_t umi_context_route_store_count(const UmiContextRouteStore *store);
/**
 * Provide the context route store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_route_store_snapshot(const UmiContextRouteStore *store,UmiContextRoute *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
