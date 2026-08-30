/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/contract.h
 *
 * PURPOSE:
 *   Define the final Framework contract for a thin Umicom Studio host.
 *
 * CONTRACT MODEL:
 *   Studio requires the Framework Studio shell persona, integrated-development
 *   perspective, Studio surface/layout catalogues, operational IDE commands,
 *   AI Developer commands and compatibility aliases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_CONTRACT_H
#define UMICOM_STUDIO_RUNTIME_CONTRACT_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeContract {
    const char *application_id;
    const char *title;
    const char *shell_persona_id;
    const char *perspective_id;
    const char *default_layout_id;
    size_t required_surface_count;
    size_t required_ide_command_count;
    size_t required_ai_command_count;
    size_t required_alias_count;
    size_t required_studio_command_count;
    size_t required_layout_count;
} UmiStudioRuntimeContract;

const UmiStudioRuntimeContract *umi_studio_runtime_contract(void);

UmiStatus umi_studio_runtime_contract_validate(
    const UmiStudioRuntimeContract *contract);

#ifdef __cplusplus
}
#endif
#endif
