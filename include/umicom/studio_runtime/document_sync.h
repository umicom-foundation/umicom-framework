/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/document_sync.h
 *
 * PURPOSE:
 *   Synchronize authoritative active-document state with Studio shell focus and
 *   window-title presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_DOCUMENT_SYNC_H
#define UMICOM_STUDIO_RUNTIME_DOCUMENT_SYNC_H

#include "umicom/studio_runtime/bindings.h"
#include "umicom/studio_runtime/document_state.h"
#include "umicom/studio_runtime/surface_activator.h"
#include "umicom/studio_runtime/window_title.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime document sync data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeDocumentSync {
    UmiStudioRuntimeDocumentState documents;
    UmiStudioRuntimeWindowTitle window_title;
    char workspace_name[256];
    uint64_t revision;
} UmiStudioRuntimeDocumentSync;

/**
 * Initialise studio document sync from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_studio_document_sync_init(
    UmiStudioRuntimeDocumentSync *sync,
    const char *workspace_name);

/**
 * Provide the studio document sync refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_document_sync_refresh(
    UmiStudioRuntimeDocumentSync *sync,
    UmiStudioRuntimeBindings *bindings);

#ifdef __cplusplus
}
#endif
#endif
