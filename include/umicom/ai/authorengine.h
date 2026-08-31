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
    /* Executable identifies the independently installed Author Engine tool. */
    char executable[UMI_AI_TEXT_CAPACITY];
    /* Workspace becomes the child process working directory, not a shell token. */
    char workspace[UMI_AI_TEXT_CAPACITY];
    /* Provider selects governed model orchestration for AI-only commands. */
    char provider[UMI_AI_ID_CAPACITY];
} UmiAiAuthorEngineConfig;

#define UMI_AI_AUTHOR_ENGINE_ARGUMENT_MAX 16U

typedef enum UmiAiAuthorEngineCommand {
    UMI_AI_AUTHOR_ENGINE_HEALTH = 1,
    UMI_AI_AUTHOR_ENGINE_CATALOGUE = 2,
    UMI_AI_AUTHOR_ENGINE_GENERATE = 3,
    /* Publication commands mirror the independently executable authoring tool. */
    UMI_AI_AUTHOR_ENGINE_PROJECT_INIT = 4,
    UMI_AI_AUTHOR_ENGINE_INGEST = 5,
    UMI_AI_AUTHOR_ENGINE_BUILD = 6,
    UMI_AI_AUTHOR_ENGINE_EXPORT = 7,
    UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW = 8,
    UMI_AI_AUTHOR_ENGINE_OPEN_PREVIEW = 9,
    UMI_AI_AUTHOR_ENGINE_RENDER = 10,
    UMI_AI_AUTHOR_ENGINE_DOCTOR = 11,
    UMI_AI_AUTHOR_ENGINE_PUBLISH = 12
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
    /* Appended for ABI growth: launchers change directory without using a shell. */
    char working_directory[UMI_AI_TEXT_CAPACITY];
} UmiAiAuthorEngineInvocation;

/* Validate configuration used by AI generation commands. A provider is
 * required because these commands select a governed model runtime. */
UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config);

/* Build an argv-only plan for AI health, catalogue or generation operations.
 * Returned text is owned by out_invocation and remains valid until overwritten. */
UmiStatus umi_ai_authorengine_plan_invocation(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    const char *session_id,
    const char *model_id,
    const char *input_path,
    const char *output_path,
    UmiAiAuthorEngineInvocation *out_invocation);

/* Build an argv-only plan for book, course, site and document publication.
 * The optional site_path is used only by the preview server. The process
 * adapter must launch in invocation.working_directory and must not use a shell. */
UmiStatus umi_ai_authorengine_plan_publication_invocation(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    const char *site_path,
    UmiAiAuthorEngineInvocation *out_invocation);

#ifdef __cplusplus
}
#endif

#endif
