/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/command_context.h
 *
 * PURPOSE:
 *   Carry host-supplied command arguments for AI developer operations without
 *   embedding toolkit event objects in Framework APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_CONTEXT_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_CONTEXT_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperCommandContext {
    char chat_session_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char approval_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char approved_by[UMI_AI_ID_CAPACITY];
    char patch_id[UMI_AI_ID_CAPACITY];
    size_t file_index;
} UmiAiDeveloperCommandContext;

void umi_ai_developer_command_context_init(
    UmiAiDeveloperCommandContext *context);

#ifdef __cplusplus
}
#endif
#endif
