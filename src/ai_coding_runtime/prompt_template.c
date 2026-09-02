/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompt_template.c
 *
 * PURPOSE:
 *   Select task prompts and define the strict complete-file response protocol
 *   used by edit-producing coding tasks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompt_template.h"

#include "umicom/ai_coding_runtime/prompts/chat.h"
#include "umicom/ai_coding_runtime/prompts/complete.h"
#include "umicom/ai_coding_runtime/prompts/explain.h"
#include "umicom/ai_coding_runtime/prompts/refactor.h"
#include "umicom/ai_coding_runtime/prompts/generate_tests.h"

/*
 * Provide the ai coding prompt system operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_prompt_system(UmiAiCodingTaskKind task)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (task) {
        case UMI_AI_CODING_TASK_CHAT:
            return umi_ai_coding_prompt_chat();
        case UMI_AI_CODING_TASK_COMPLETE:
            return umi_ai_coding_prompt_complete();
        case UMI_AI_CODING_TASK_EXPLAIN:
            return umi_ai_coding_prompt_explain();
        case UMI_AI_CODING_TASK_REFACTOR:
            return umi_ai_coding_prompt_refactor();
        case UMI_AI_CODING_TASK_GENERATE_TESTS:
            return umi_ai_coding_prompt_generate_tests();
        default:
            return NULL;
    }
}

/*
 * Provide the ai coding prompt task may modify operation used by this module and its
 * client applications.
 */
int umi_ai_coding_prompt_task_may_modify(UmiAiCodingTaskKind task)
{
    return task == UMI_AI_CODING_TASK_REFACTOR ||
        task == UMI_AI_CODING_TASK_GENERATE_TESTS;
}

/*
 * Provide the ai coding prompt response protocol operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_prompt_response_protocol(void)
{
    return
        "For edit-producing tasks, reply using exactly this protocol. "
        "Use repository-relative paths and full replacement text for CREATE or "
        "MODIFY files. Do not use markdown fences around protocol records.\n"
        "UMICOM-CODING-RESPONSE/1\n"
        "SUMMARY-BEGIN\n"
        "<short human-readable summary>\n"
        "SUMMARY-END\n"
        "PATCH-BEGIN\n"
        "TITLE|<short patch title>\n"
        "RATIONALE-BEGIN\n"
        "<why these changes are needed>\n"
        "RATIONALE-END\n"
        "FILE-BEGIN|CREATE|relative/path\n"
        "CONTENT-BEGIN\n"
        "<complete file content>\n"
        "CONTENT-END\n"
        "FILE-END\n"
        "FILE-BEGIN|MODIFY|relative/path\n"
        "CONTENT-BEGIN\n"
        "<complete file content>\n"
        "CONTENT-END\n"
        "FILE-END\n"
        "FILE-BEGIN|DELETE|relative/path\n"
        "FILE-END\n"
        "PATCH-END\n"
        "For a task that requires no edit, omit PATCH-BEGIN through PATCH-END.";
}
