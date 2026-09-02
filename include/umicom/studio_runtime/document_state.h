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

/**
 * Represent the studio runtime document state data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeDocumentState {
    UmiDocumentWorkingCopySnapshot active;
    size_t open_document_count;
    uint64_t revision;
    int has_active;
    int external_change_detected;
} UmiStudioRuntimeDocumentState;

/**
 * Initialise studio document state from caller-provided values so later operations receive
 * a known state.
 */
void umi_studio_document_state_init(
    UmiStudioRuntimeDocumentState *state);

/**
 * Provide the studio document state refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_document_state_refresh(
    UmiStudioRuntimeDocumentState *state,
    UmiDocumentCoordinator *documents);

#ifdef __cplusplus
}
#endif
#endif
