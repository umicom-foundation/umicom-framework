/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/badge_sync.h
 *
 * PURPOSE:
 *   Synchronize Problems, Source Control, Testing and AI approval badges onto
 *   the existing Studio activity/sidebar contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_BADGE_SYNC_H
#define UMICOM_STUDIO_RUNTIME_BADGE_SYNC_H

#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the studio badge sync operation used by this module and its client applications.
 */
UmiStatus umi_studio_badge_sync(
    UmiStudioRuntimeBindings *bindings,
    const UmiIdeIntegrationPlatformSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
