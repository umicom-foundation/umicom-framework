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

typedef struct UmiApplicationProductionCommandBinding {
    const UmiApplicationCommandDescriptor *command;
    int enabled;
    int mutates_workspace;
} UmiApplicationProductionCommandBinding;

typedef struct UmiApplicationProductionCommandBindings {
    UmiApplicationCommandSurface surface;
    UmiApplicationProductionCommandBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_COMMANDS];
    size_t count;
    size_t enabled_count;
} UmiApplicationProductionCommandBindings;

UmiStatus umi_application_production_command_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCommandBindings *out_bindings);
const UmiApplicationProductionCommandBinding *
umi_application_production_command_bindings_find(
    const UmiApplicationProductionCommandBindings *bindings,
    UmiApplicationCommandKind kind, const char *target_id);

#ifdef __cplusplus
}
#endif
#endif
