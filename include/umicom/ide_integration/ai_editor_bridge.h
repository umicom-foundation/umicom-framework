/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/ai_editor_bridge.h
 *
 * PURPOSE:
 *   Build governed AI Coding Runtime requests from active editor context.
 *
 * COMPATIBILITY:
 *   The original bridge remains source-compatible. A new runtime-aware entry
 *   point lets Framework applications supply an explicit runtime identifier;
 *   legacy callers deterministically use their session identifier as the
 *   runtime identifier so requests satisfy the current governed AI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_AI_EDITOR_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_AI_EDITOR_BRIDGE_H

#include "umicom/ai/coding_types.h"
#include "umicom/ide_integration/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide ai request from selection runtime operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_ai_request_from_selection_runtime(
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *request_id,
    const char *session_id,
    const char *runtime_id,
    const char *workspace_root,
    const char *instruction,
    UmiAiCodingRequest *out_request);

/**
 * Provide the ide ai request from selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_ai_request_from_selection(
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *request_id,
    const char *session_id,
    const char *workspace_root,
    const char *instruction,
    UmiAiCodingRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
