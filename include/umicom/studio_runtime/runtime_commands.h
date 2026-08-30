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

typedef struct UmiStudioRuntimeCommandService
    UmiStudioRuntimeCommandService;

size_t umi_studio_runtime_command_count(void);
const char *umi_studio_runtime_command_id_at(size_t index);

UmiStatus umi_studio_runtime_command_service_create(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSessionController *session,
    UmiStudioRuntimeCommandService **out_service);

void umi_studio_runtime_command_service_destroy(
    UmiStudioRuntimeCommandService *service);

UmiStatus umi_studio_runtime_command_service_set_session(
    UmiStudioRuntimeCommandService *service,
    UmiStudioRuntimeSessionController *session);

UmiStatus umi_studio_runtime_command_service_register(
    UmiStudioRuntimeCommandService *service);

size_t umi_studio_runtime_command_service_registered_count(
    const UmiStudioRuntimeCommandService *service);

#ifdef __cplusplus
}
#endif
#endif
