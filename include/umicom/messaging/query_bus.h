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

typedef struct UmiQueryBus UmiQueryBus;
typedef UmiStatus (*UmiQueryHandler)(const char *request, char *response,
                                     size_t response_capacity, void *user_data);

UmiStatus umi_query_bus_create(UmiQueryBus **out_bus);
void umi_query_bus_destroy(UmiQueryBus *bus);
UmiStatus umi_query_bus_register(UmiQueryBus *bus, const char *name,
                                 UmiQueryHandler handler, void *user_data);
UmiStatus umi_query_bus_execute(UmiQueryBus *bus, const char *name,
                                const char *request, char *response,
                                size_t response_capacity);

#ifdef __cplusplus
}
#endif

#endif
