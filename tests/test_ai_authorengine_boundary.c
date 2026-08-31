/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_authorengine_boundary.c
 *
 * PURPOSE:
 *   Verify AuthorEngine stays behind a deterministic argv process boundary.
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

#include "umicom/ai/authorengine.h"

int main(void)
{
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineInvocation invocation;
    (void)strcpy(config.executable, "C:\\Program Files\\Umicom\\uaengine.exe");
    (void)strcpy(config.workspace, "C:\\Dev\\My Workspace");
    (void)strcpy(config.provider, "authorengine.local");
    assert(umi_ai_authorengine_plan_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_GENERATE, "session-47", "small-model",
        "input with spaces.json", "output with spaces.json", &invocation)
        == UMI_STATUS_OK);
    assert(strcmp(invocation.executable, config.executable) == 0);
    assert(strcmp(invocation.working_directory, config.workspace) == 0);
    assert(invocation.argument_count == 13U);
    assert(strcmp(invocation.arguments[0], "generate") == 0);
    assert(strcmp(invocation.arguments[2], config.workspace) == 0);
    assert(strcmp(invocation.arguments[6], "session-47") == 0);
    assert(strcmp(invocation.arguments[12], "output with spaces.json") == 0);
    assert(umi_ai_authorengine_plan_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_GENERATE, NULL, "model", "in", "out",
        &invocation) == UMI_STATUS_INVALID_ARGUMENT);
    /* Publication uses the real project working directory and no provider flags. */
    assert(umi_ai_authorengine_plan_publication_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_BUILD, NULL, &invocation)
        == UMI_STATUS_OK);
    assert(invocation.argument_count == 1U);
    assert(strcmp(invocation.arguments[0], "build") == 0);
    assert(strcmp(invocation.working_directory, config.workspace) == 0);
    /* Preview paths remain one argv value even when the directory has spaces. */
    assert(umi_ai_authorengine_plan_publication_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW,
        "output/my book/site", &invocation) == UMI_STATUS_OK);
    assert(invocation.argument_count == 3U);
    assert(strcmp(invocation.arguments[1], "--site") == 0);
    assert(strcmp(invocation.arguments[2], "output/my book/site") == 0);
    /* A site override on a non-preview command is rejected as ambiguous input. */
    assert(umi_ai_authorengine_plan_publication_invocation(
        &config, UMI_AI_AUTHOR_ENGINE_EXPORT, "unexpected", &invocation)
        == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
