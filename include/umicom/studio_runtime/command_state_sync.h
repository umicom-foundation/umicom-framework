/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_state_sync.h
 *
 * PURPOSE:
 *   Reconcile Application Shell enablement against the authoritative runtime
 *   command registry after context changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_STATE_SYNC_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_STATE_SYNC_H

#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime command sync report data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeCommandSyncReport {
    size_t contribution_count;
    size_t command_backed_count;
    size_t enabled_count;
    size_t disabled_count;
    uint64_t shell_revision;
} UmiStudioRuntimeCommandSyncReport;

/**
 * Provide the studio runtime sync commands operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_sync_commands(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandSyncReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
