/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/session_bundle.h
 *
 * PURPOSE:
 *   Persist and restore the coordinated AI developer experience state through a
 *   single existing UmiSessionStore namespace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_SESSION_BUNDLE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_SESSION_BUNDLE_H

#include "umicom/ai_developer_experience/approval_persistence.h"
#include "umicom/ai_developer_experience/chat_persistence.h"
#include "umicom/ai_developer_experience/preferences_persistence.h"
#include "umicom/ai_developer_experience/presentation_persistence.h"
#include "umicom/ai_developer_experience/restore_report.h"
#include "umicom/ai_developer_experience/state_schema.h"
#include "umicom/ai_developer_experience/task_persistence.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write ai developer session bundle in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_session_bundle_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatRegistry *chats,
    const UmiAiDeveloperTaskRegistry *tasks,
    const UmiAiDeveloperApprovalQueue *approvals,
    const UmiAiDeveloperPresentationState *presentation,
    const UmiAiDeveloperPreferences *preferences);

/**
 * Provide the ai developer session bundle restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_session_bundle_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatRegistry *chats,
    UmiAiDeveloperTaskRegistry *tasks,
    UmiAiDeveloperApprovalQueue *approvals,
    UmiAiDeveloperPresentationState *presentation,
    UmiAiDeveloperPreferences *preferences,
    UmiAiDeveloperRestoreReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
