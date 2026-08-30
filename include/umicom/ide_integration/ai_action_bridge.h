/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/ai_action_bridge.h
 *
 * PURPOSE:
 *   Route editor/Problems actions into the operational governed AI Coding Runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_AI_ACTION_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_AI_ACTION_BRIDGE_H

#include "umicom/ide_integration/ai_editor_bridge.h"
#include "umicom/ide_integration/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ide_ai_run_selection(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *task_id,
    const char *request_id,
    const char *session_id,
    const char *instruction);

UmiStatus umi_ide_ai_fix_problem(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeEditorSelection *selection,
    size_t problem_index,
    const char *task_id,
    const char *request_id,
    const char *session_id);

#ifdef __cplusplus
}
#endif
#endif
