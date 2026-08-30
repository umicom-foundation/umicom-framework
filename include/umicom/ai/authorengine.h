/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/authorengine.h
 *
 * PURPOSE:
 *   Configure the Umicom AuthorEngine executable and build deterministic CLI arguments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * AuthorEngine is integrated through a process boundary, so Studio can reuse its capabilities without copying publishing logic into the IDE.
 */

#ifndef INCLUDE_UMICOM_AI_AUTHORENGINE_H
#define INCLUDE_UMICOM_AI_AUTHORENGINE_H

#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiAuthorEngineConfig {
    char executable[UMI_AI_TEXT_CAPACITY];
    char workspace[UMI_AI_TEXT_CAPACITY];
    char provider[UMI_AI_ID_CAPACITY];
} UmiAiAuthorEngineConfig;

#define UMI_AI_AUTHOR_ENGINE_ARGUMENT_MAX 16U

typedef enum UmiAiAuthorEngineCommand {
    UMI_AI_AUTHOR_ENGINE_HEALTH = 1,
    UMI_AI_AUTHOR_ENGINE_CATALOGUE = 2,
    UMI_AI_AUTHOR_ENGINE_GENERATE = 3
} UmiAiAuthorEngineCommand;

/*
 * Process adapters execute this argv plan directly.  The Framework never
 * builds one shell command string, which keeps paths containing spaces safe
 * and lets Windows and POSIX launchers apply their own escaping rules.
 */
typedef struct UmiAiAuthorEngineInvocation {
    char executable[UMI_AI_TEXT_CAPACITY];
    char arguments[UMI_AI_AUTHOR_ENGINE_ARGUMENT_MAX]
                  [UMI_AI_SMALL_TEXT_CAPACITY];
    size_t argument_count;
    UmiAiAuthorEngineCommand command;
} UmiAiAuthorEngineInvocation;

UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config);
UmiStatus umi_ai_authorengine_plan_invocation(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    const char *session_id,
    const char *model_id,
    const char *input_path,
    const char *output_path,
    UmiAiAuthorEngineInvocation *out_invocation);

#ifdef __cplusplus
}
#endif

#endif
