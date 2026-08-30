/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/document_state.h
 *
 * PURPOSE:
 *   Project authoritative DocumentCoordinator state into a compact Studio
 *   presentation record without duplicating document text or undo history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_DOCUMENT_STATE_H
#define UMICOM_STUDIO_RUNTIME_DOCUMENT_STATE_H

#include "umicom/document/coordinator.h"
#include "umicom/studio_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeDocumentState {
    UmiDocumentWorkingCopySnapshot active;
    size_t open_document_count;
    uint64_t revision;
    int has_active;
    int external_change_detected;
} UmiStudioRuntimeDocumentState;

void umi_studio_document_state_init(
    UmiStudioRuntimeDocumentState *state);

UmiStatus umi_studio_document_state_refresh(
    UmiStudioRuntimeDocumentState *state,
    UmiDocumentCoordinator *documents);

#ifdef __cplusplus
}
#endif
#endif
