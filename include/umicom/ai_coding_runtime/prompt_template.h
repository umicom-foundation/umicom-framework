/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/prompt_template.h
 *
 * PURPOSE:
 *   Publish stable task-specific prompt contracts for coding chat, completion,
 *   explanation, refactoring and test generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PROMPT_TEMPLATE_H
#define UMICOM_AI_CODING_RUNTIME_PROMPT_TEMPLATE_H

#include "umicom/ai_coding_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding prompt system operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_prompt_system(UmiAiCodingTaskKind task);
/**
 * Provide the ai coding prompt response protocol operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_prompt_response_protocol(void);
/**
 * Provide the ai coding prompt task may modify operation used by this module and its
 * client applications.
 */
int umi_ai_coding_prompt_task_may_modify(UmiAiCodingTaskKind task);

#ifdef __cplusplus
}
#endif
#endif
