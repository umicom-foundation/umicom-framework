/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command_router.h
 *
 * PURPOSE:
 *   Execute stable IDE integration commands against the composed Framework
 *   platform and current toolkit-neutral command context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_COMMAND_ROUTER_H
#define UMICOM_IDE_INTEGRATION_COMMAND_ROUTER_H

#include "umicom/ide_integration/ai_action_bridge.h"
#include "umicom/ide_integration/command.h"
#include "umicom/ide_integration/command_context.h"
#include "umicom/ide_integration/workflow_profiles/profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeCommandRouter {
    UmiIdeIntegrationPlatform *platform;
    UmiIdeCommandContext context;
    uint64_t sequence;
} UmiIdeCommandRouter;

UmiStatus umi_ide_command_router_init(
    UmiIdeCommandRouter *router,
    UmiIdeIntegrationPlatform *platform);

UmiStatus umi_ide_command_router_set_context(
    UmiIdeCommandRouter *router,
    const UmiIdeCommandContext *context);

int umi_ide_command_router_enabled(
    UmiIdeCommandRouter *router,
    const char *command_id);

UmiStatus umi_ide_command_router_execute(
    UmiIdeCommandRouter *router,
    const char *command_id,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
