/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_ui/assistant_windows.h
 *
 * PURPOSE:
 *   Register reusable AI chat, agent, approval, activity, comparison and
 *   research windows in an application's New Window catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An application opts into these shared window definitions after composing an
 * AI runtime. The application remains responsible only for its provider and
 * workspace configuration; Framework owns the common window vocabulary.
 */
#ifndef UMICOM_AI_UI_ASSISTANT_WINDOWS_H
#define UMICOM_AI_UI_ASSISTANT_WINDOWS_H

#include "umicom/ui/window_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_UI_WINDOW_CHAT "umicom.ai.chat"
#define UMI_AI_UI_WINDOW_AGENT_TASKS "umicom.ai.agent-tasks"
#define UMI_AI_UI_WINDOW_APPROVALS "umicom.ai.approvals"
#define UMI_AI_UI_WINDOW_TOOL_ACTIVITY "umicom.ai.tool-activity"
#define UMI_AI_UI_WINDOW_MODEL_COMPARISON "umicom.ai.model-comparison"
#define UMI_AI_UI_WINDOW_RESEARCH_CONTEXT "umicom.ai.research-context"

UmiStatus umi_ai_ui_assistant_windows_register(
    UmiUiWindowCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
