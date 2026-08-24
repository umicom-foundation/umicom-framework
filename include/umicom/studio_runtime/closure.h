/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/closure.h
 *
 * PURPOSE:
 *   Measure runtime closure of the thin-Studio contract against actual shell
 *   contributions and registered commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_CLOSURE_H
#define UMICOM_STUDIO_RUNTIME_CLOSURE_H

#include "umicom/application_shell/registry.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/studio_runtime/contract.h"
#include "umicom/studio_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_RUNTIME_MISSING_CAPACITY 96U

typedef struct UmiStudioRuntimeClosureReport {
    char missing_surfaces[UMI_STUDIO_RUNTIME_MISSING_CAPACITY]
                         [UMI_STUDIO_RUNTIME_ID_CAPACITY];
    char missing_commands[UMI_STUDIO_RUNTIME_MISSING_CAPACITY]
                         [UMI_STUDIO_RUNTIME_ID_CAPACITY];
    size_t missing_surface_count;
    size_t missing_command_count;
    size_t installed_surface_count;
    size_t registered_ide_command_count;
    size_t registered_ai_command_count;
    size_t registered_alias_count;
    size_t registered_studio_command_count;
    size_t layout_count;
    int persona_available;
    int perspective_available;
    int default_layout_available;
    int ready;
    uint64_t revision;
} UmiStudioRuntimeClosureReport;

UmiStatus umi_studio_runtime_closure_check(
    const UmiStudioRuntimeContract *contract,
    UmiApplicationShellRegistry *shell,
    const UmiCommandRegistry *commands,
    UmiStudioRuntimeClosureReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
