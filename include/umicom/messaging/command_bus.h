/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/command_bus.h
 *
 * PURPOSE:
 *   Publish the public command bus contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_COMMAND_BUS_H
#define UMICOM_MESSAGING_COMMAND_BUS_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommandBus UmiCommandBus;
typedef UmiStatus (*UmiCommandHandler)(const char *payload, void *user_data);

UmiStatus umi_command_bus_create(UmiCommandBus **out_bus);
void umi_command_bus_destroy(UmiCommandBus *bus);
UmiStatus umi_command_bus_register(UmiCommandBus *bus, const char *name,
                                   UmiCommandHandler handler, void *user_data);
UmiStatus umi_command_bus_execute(UmiCommandBus *bus, const char *name,
                                  const char *payload);

#ifdef __cplusplus
}
#endif

#endif
