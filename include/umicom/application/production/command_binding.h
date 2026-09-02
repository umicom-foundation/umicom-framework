/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/command_binding.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_COMMAND_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_COMMAND_BINDING_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"
#include "umicom/application/runtime/command_surface.h"

/**
 * Represent the application production command binding data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionCommandBinding {
    const UmiApplicationCommandDescriptor *command;
    int enabled;
    int mutates_workspace;
} UmiApplicationProductionCommandBinding;

/**
 * Represent the application production command bindings data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionCommandBindings {
    UmiApplicationCommandSurface surface;
    UmiApplicationProductionCommandBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_COMMANDS];
    size_t count;
    size_t enabled_count;
} UmiApplicationProductionCommandBindings;

/**
 * Provide the application production command bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_command_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCommandBindings *out_bindings);
/**
 * Find application production command bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionCommandBinding *
umi_application_production_command_bindings_find(
    const UmiApplicationProductionCommandBindings *bindings,
    UmiApplicationCommandKind kind, const char *target_id);

#ifdef __cplusplus
}
#endif
#endif
