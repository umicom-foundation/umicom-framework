/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_types.c
 *
 * PURPOSE:
 *   Verify stable coding task vocabulary, request validation, path containment
 *   and deterministic content hashes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/coding_types.h"

int main(void)
{
    UmiAiCodingRequest request;
    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_REFACTOR);
    (void)strcpy(request.request_id, "request.48");
    (void)strcpy(request.session_id, "studio.session.default");
    (void)strcpy(request.runtime_id, "authorengine.local.chat");
    (void)strcpy(request.workspace_root, "C:\\Dev\\umicom\\umicom-studio");
    (void)strcpy(request.active_path, "applications/studio/src/app/main.c");
    (void)strcpy(request.language_id, "c23");
    (void)strcpy(request.instruction, "Extract the repeated validation logic.");
    request.selection_start_line = 10U;
    request.selection_end_line = 30U;

    assert(umi_ai_coding_request_validate(&request) == UMI_STATUS_OK);
    assert(umi_ai_coding_path_is_safe_relative("src/app.c"));
    assert(umi_ai_coding_path_is_safe_relative("src\\app.c"));
    assert(!umi_ai_coding_path_is_safe_relative("../secret.txt"));
    assert(!umi_ai_coding_path_is_safe_relative("C:\\secret.txt"));
    assert(!umi_ai_coding_path_is_safe_relative("/etc/passwd"));
    assert(umi_ai_coding_text_hash("abc", 3U) ==
           umi_ai_coding_text_hash("abc", 3U));
    assert(strcmp(umi_ai_coding_task_kind_text(request.task), "refactoring") == 0);
    return 0;
}
