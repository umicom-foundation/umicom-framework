/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/ai_editor_bridge.h
 *
 * PURPOSE:
 *   Build governed AI Coding Runtime requests from active editor context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_AI_EDITOR_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_AI_EDITOR_BRIDGE_H

#include "umicom/ai/coding_types.h"
#include "umicom/ide_integration/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

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
