/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/document_sync.h
 *
 * PURPOSE:
 *   Synchronize authoritative active-document state with Studio shell focus and
 *   window-title presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiStudioRuntimeDocumentSync {
    UmiStudioRuntimeDocumentState documents;
    UmiStudioRuntimeWindowTitle window_title;
    char workspace_name[256];
    uint64_t revision;
} UmiStudioRuntimeDocumentSync;

UmiStatus umi_studio_document_sync_init(
    UmiStudioRuntimeDocumentSync *sync,
    const char *workspace_name);

UmiStatus umi_studio_document_sync_refresh(
    UmiStudioRuntimeDocumentSync *sync,
    UmiStudioRuntimeBindings *bindings);

#ifdef __cplusplus
}
#endif
#endif
