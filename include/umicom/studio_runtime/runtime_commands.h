/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/runtime_commands.h
 *
 * PURPOSE:
 *   Register Studio composition commands for layout switching, semantic layout
 *   persistence, refresh, command search, close checks and contract closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_RUNTIME_COMMANDS_H
#define UMICOM_STUDIO_RUNTIME_RUNTIME_COMMANDS_H

#include "umicom/studio_runtime/close_guard.h"
#include "umicom/studio_runtime/closure.h"
#include "umicom/studio_runtime/command_search.h"
#include "umicom/studio_runtime/session_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime command service data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeCommandService
    UmiStudioRuntimeCommandService;

/**
 * Return the number of records represented by studio runtime command without changing
 * their state.
 */
size_t umi_studio_runtime_command_count(void);
/**
 * Find studio runtime command id while leaving the underlying catalogue or model owned by
 * this module.
 */
const char *umi_studio_runtime_command_id_at(size_t index);

/**
 * Initialise studio runtime command service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_studio_runtime_command_service_create(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSessionController *session,
    UmiStudioRuntimeCommandService **out_service);

/**
 * Release or reset state held by studio runtime command service so the same storage can be
 * reused safely.
 */
void umi_studio_runtime_command_service_destroy(
    UmiStudioRuntimeCommandService *service);

/**
 * Provide the studio runtime command service set session operation used by this module and
 * its client applications.
 */
UmiStatus umi_studio_runtime_command_service_set_session(
    UmiStudioRuntimeCommandService *service,
    UmiStudioRuntimeSessionController *session);

/**
 * Add studio runtime command service only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_studio_runtime_command_service_register(
    UmiStudioRuntimeCommandService *service);

/**
 * Return the number of records represented by studio runtime command service registered
 * without changing their state.
 */
size_t umi_studio_runtime_command_service_registered_count(
    const UmiStudioRuntimeCommandService *service);

#ifdef __cplusplus
}
#endif
#endif
