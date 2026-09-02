/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/query_bus.h
 *
 * PURPOSE:
 *   Publish the public query bus contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_QUERY_BUS_H
#define UMICOM_MESSAGING_QUERY_BUS_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the query bus data shared with callers of this public contract.
 */
typedef struct UmiQueryBus UmiQueryBus;
typedef UmiStatus (*UmiQueryHandler)(const char *request, char *response,
                                     size_t response_capacity, void *user_data);

/**
 * Initialise query bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_query_bus_create(UmiQueryBus **out_bus);
/**
 * Release or reset state held by query bus so the same storage can be reused safely.
 */
void umi_query_bus_destroy(UmiQueryBus *bus);
/**
 * Add query bus only after its inputs and available capacity have been checked.
 */
UmiStatus umi_query_bus_register(UmiQueryBus *bus, const char *name,
                                 UmiQueryHandler handler, void *user_data);
/**
 * Perform query bus through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_query_bus_execute(UmiQueryBus *bus, const char *name,
                                const char *request, char *response,
                                size_t response_capacity);

#ifdef __cplusplus
}
#endif

#endif
