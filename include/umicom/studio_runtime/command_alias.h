/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_alias.h
 *
 * PURPOSE:
 *   Define compatibility aliases that connect long-lived Application Shell menu/
 *   toolbar command IDs to the newer operational IDE integration commands and
 *   Studio surfaces.
 *
 * DESIGN:
 *   Aliases never replace an already-registered command. Existing operational
 *   commands always win.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_H

#include "umicom/studio_runtime/types.h"
#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime command alias definition data shared with callers of this
 * public contract.
 */
typedef struct UmiStudioRuntimeCommandAliasDefinition {
    const char *alias_id;
    const char *title;
    const char *category;
    const char *description;
    const char *target_command_id;
    UmiStudioRuntimeSurfaceKind surface;
    int activate_surface;
    int mutates_state;
} UmiStudioRuntimeCommandAliasDefinition;

/**
 * Check that studio command alias satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_studio_command_alias_validate(
    const UmiStudioRuntimeCommandAliasDefinition *definition);

#ifdef __cplusplus
}
#endif
#endif
