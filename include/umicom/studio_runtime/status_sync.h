/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/status_sync.h
 *
 * PURPOSE:
 *   Synchronize Framework Studio status items into Application Shell status-bar
 *   contributions, creating only the missing Studio-specific items.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_STATUS_SYNC_H
#define UMICOM_STUDIO_RUNTIME_STATUS_SYNC_H

#include "umicom/studio_runtime/bindings.h"
#include "umicom/studio_runtime/status_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the studio status sync operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_status_sync(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeStatusModel *model);

#ifdef __cplusplus
}
#endif
#endif
