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

/**
 * Represent the command bus data shared with callers of this public contract.
 */
typedef struct UmiCommandBus UmiCommandBus;
typedef UmiStatus (*UmiCommandHandler)(const char *payload, void *user_data);

/**
 * Initialise command bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_command_bus_create(UmiCommandBus **out_bus);
/**
 * Release or reset state held by command bus so the same storage can be reused safely.
 */
void umi_command_bus_destroy(UmiCommandBus *bus);
/**
 * Add command bus only after its inputs and available capacity have been checked.
 */
UmiStatus umi_command_bus_register(UmiCommandBus *bus, const char *name,
                                   UmiCommandHandler handler, void *user_data);
/**
 * Perform command bus through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_command_bus_execute(UmiCommandBus *bus, const char *name,
                                  const char *payload);

#ifdef __cplusplus
}
#endif

#endif
