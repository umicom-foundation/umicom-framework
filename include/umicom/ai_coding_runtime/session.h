/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/session.h
 *
 * PURPOSE:
 *   Retain reusable coding-agent session identity, active workspace/task and
 *   interaction counters independently of any Studio window or chat widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_SESSION_H
#define UMICOM_AI_CODING_RUNTIME_SESSION_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding session data shared with callers of this public contract.
 */
typedef struct UmiAiCodingSession {
    char session_id[UMI_AI_ID_CAPACITY];
    char workspace_root[UMI_AI_TEXT_CAPACITY];
    char active_task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    uint64_t task_count;
    uint64_t completed_count;
    uint64_t failed_count;
    uint64_t cancelled_count;
    uint64_t revision;
} UmiAiCodingSession;

/**
 * Initialise ai coding session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_session_init(
    UmiAiCodingSession *session,
    const char *session_id,
    const char *workspace_root);

/**
 * Provide the ai coding session begin task operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_session_begin_task(
    UmiAiCodingSession *session,
    const char *task_id);

/**
 * Provide the ai coding session complete task operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_session_complete_task(
    UmiAiCodingSession *session,
    UmiAiCodingRuntimeState final_state);

#ifdef __cplusplus
}
#endif
#endif
