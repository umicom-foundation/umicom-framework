/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/prompt_template.h
 *
 * PURPOSE:
 *   Publish stable task-specific prompt contracts for coding chat, completion,
 *   explanation, refactoring and test generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PROMPT_TEMPLATE_H
#define UMICOM_AI_CODING_RUNTIME_PROMPT_TEMPLATE_H

#include "umicom/ai_coding_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *umi_ai_coding_prompt_system(UmiAiCodingTaskKind task);
const char *umi_ai_coding_prompt_response_protocol(void);
int umi_ai_coding_prompt_task_may_modify(UmiAiCodingTaskKind task);

#ifdef __cplusplus
}
#endif
#endif
