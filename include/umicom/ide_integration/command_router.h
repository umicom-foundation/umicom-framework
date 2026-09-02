/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command_router.h
 *
 * PURPOSE:
 *   Execute stable IDE integration commands against the composed Framework
 *   platform and current toolkit-neutral command context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ide command router data shared with callers of this public contract.
 */
typedef struct UmiIdeCommandRouter {
    UmiIdeIntegrationPlatform *platform;
    UmiIdeCommandContext context;
    uint64_t sequence;
} UmiIdeCommandRouter;

/**
 * Initialise ide command router from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_command_router_init(
    UmiIdeCommandRouter *router,
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide command router set context operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_command_router_set_context(
    UmiIdeCommandRouter *router,
    const UmiIdeCommandContext *context);

/**
 * Provide the ide command router enabled operation used by this module and its client
 * applications.
 */
int umi_ide_command_router_enabled(
    UmiIdeCommandRouter *router,
    const char *command_id);

/**
 * Perform ide command router through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ide_command_router_execute(
    UmiIdeCommandRouter *router,
    const char *command_id,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
