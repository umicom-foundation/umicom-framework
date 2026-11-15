/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_authorengine_planning.c
 *
 * PURPOSE:
 *   Verify complete native argv planning, bounded publication metadata, refusal preservation and inputs borrowed from previous output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/authorengine.h"

int main(int argc, char **argv)
{
    static const char *const modes[] = {"publication-commands",
        "publication-no-provider",
        "publication-unused-provider",
        "publication-null-config",
        "publication-null-output",
        "publication-empty-executable",
        "publication-empty-workspace",
        "publication-executable-bound",
        "publication-workspace-bound",
        "publication-long-site",
        "publication-site-limit",
        "publication-site-invalid-command",
        "publication-command-invalid",
        "publication-alias",
        "ai-health",
        "ai-catalogue",
        "ai-generate",
        "ai-null-config",
        "ai-null-output",
        "ai-command-invalid",
        "ai-missing-input",
        "ai-long-session",
        "ai-long-model",
        "ai-long-input",
        "ai-long-output",
        "ai-long-workspace",
        "ai-argument-limit",
        "ai-alias",
        "ai-alias-refusal",
        "literal-arguments",
        "publication-field-limits"};
    static const UmiAiAuthorEngineCommand publications[] = {
        UMI_AI_AUTHOR_ENGINE_PROJECT_INIT, UMI_AI_AUTHOR_ENGINE_INGEST,
        UMI_AI_AUTHOR_ENGINE_BUILD, UMI_AI_AUTHOR_ENGINE_EXPORT,
        UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW, UMI_AI_AUTHOR_ENGINE_OPEN_PREVIEW,
        UMI_AI_AUTHOR_ENGINE_RENDER, UMI_AI_AUTHOR_ENGINE_DOCTOR, UMI_AI_AUTHOR_ENGINE_PUBLISH
    };
    static const char *const publication_names[] = {
        "init", "ingest", "build", "export", "serve", "open", "render", "doctor", "publish"
    };
    UmiAiAuthorEngineConfig config = {0}, config_before;
    UmiAiAuthorEngineInvocation invocation, before;
    char long_argument[UMI_AI_SMALL_TEXT_CAPACITY + 1U];
    char exact_argument[UMI_AI_SMALL_TEXT_CAPACITY];
    const char *session = "session.example", *model = "model.example";
    const char *input = "input with spaces.json", *output = "output with spaces.json";
    const char *site = NULL;
    UmiAiAuthorEngineCommand command = UMI_AI_AUTHOR_ENGINE_GENERATE;
    UmiStatus status, expected = UMI_STATUS_OK;
    int known = 0, publication;
    assert(argc == 2);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
#define CASE(name) (strcmp(argv[1], name) == 0)
    publication = strncmp(argv[1], "publication-", 12U) == 0;
    strcpy(config.executable, "C:/Program Files/Umicom/uaengine.exe");
    strcpy(config.workspace, "C:/Projects/Example Workspace");
    strcpy(config.provider, "provider.local");
    memset(&invocation, 0xA5, sizeof invocation);
    memset(long_argument, 'x', sizeof long_argument - 1U);
    long_argument[sizeof long_argument - 1U] = '\0';
    memset(exact_argument, 'x', sizeof exact_argument - 1U);
    exact_argument[sizeof exact_argument - 1U] = '\0';
    if (publication) {
        command = UMI_AI_AUTHOR_ENGINE_BUILD;
        if (CASE("publication-no-provider")) config.provider[0] = '\0';
        if (CASE("publication-unused-provider")) memset(config.provider, 'x', sizeof config.provider);
        if (CASE("publication-empty-executable")) config.executable[0] = '\0';
        if (CASE("publication-empty-workspace")) config.workspace[0] = '\0';
        if (CASE("publication-executable-bound")) memset(config.executable, 'x', sizeof config.executable);
        if (CASE("publication-workspace-bound")) memset(config.workspace, 'x', sizeof config.workspace);
        if (CASE("publication-long-site") || CASE("publication-site-limit") || CASE("publication-alias"))
            command = UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW;
        if (CASE("publication-long-site")) { site = long_argument; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        if (CASE("publication-site-limit")) site = exact_argument;
        if (CASE("publication-site-invalid-command")) site = "a site that build does not accept";
        if (CASE("publication-command-invalid")) command = UMI_AI_AUTHOR_ENGINE_HEALTH;
        if (CASE("publication-alias")) {
            strcpy(invocation.arguments[4], "saved site folder");
            site = invocation.arguments[4];
        }
        if (CASE("publication-field-limits")) {
            memset(config.executable, 'x', sizeof config.executable - 1U);
            config.executable[sizeof config.executable - 1U] = '\0';
            memset(config.workspace, 'y', sizeof config.workspace - 1U);
            config.workspace[sizeof config.workspace - 1U] = '\0';
        }
        if (CASE("publication-empty-executable") || CASE("publication-empty-workspace") ||
            CASE("publication-executable-bound") || CASE("publication-workspace-bound") ||
            CASE("publication-site-invalid-command") || CASE("publication-command-invalid") ||
            CASE("publication-null-config") || CASE("publication-null-output"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        if (CASE("ai-health")) command = UMI_AI_AUTHOR_ENGINE_HEALTH;
        if (CASE("ai-catalogue")) command = UMI_AI_AUTHOR_ENGINE_CATALOGUE;
        if (CASE("ai-command-invalid")) command = UMI_AI_AUTHOR_ENGINE_BUILD;
        if (CASE("ai-long-session")) session = long_argument;
        if (CASE("ai-long-model")) model = long_argument;
        if (CASE("ai-long-input")) input = long_argument;
        if (CASE("ai-long-output")) output = long_argument;
        if (CASE("ai-long-workspace")) strcpy(config.workspace, long_argument);
        if (CASE("ai-argument-limit")) output = exact_argument;
        if (CASE("ai-alias") || CASE("ai-alias-refusal")) {
            strcpy(invocation.arguments[1], "borrowed session");
            strcpy(invocation.arguments[2], "borrowed model");
            strcpy(invocation.arguments[3], "borrowed input");
            strcpy(invocation.arguments[4], "borrowed output");
            session = invocation.arguments[1]; model = invocation.arguments[2];
            input = invocation.arguments[3]; output = invocation.arguments[4];
            if (CASE("ai-alias-refusal")) output = long_argument;
        }
        if (CASE("literal-arguments")) {
            input = "file;$(literal) \"quoted\".json";
            output = "out & still one argument.json";
        }
        if (CASE("ai-long-session") || CASE("ai-long-model") || CASE("ai-long-input") ||
            CASE("ai-long-output") || CASE("ai-long-workspace") || CASE("ai-alias-refusal"))
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        if (CASE("ai-command-invalid") || CASE("ai-null-config") || CASE("ai-null-output"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(&before, &invocation, sizeof before);
    memcpy(&config_before, &config, sizeof config_before);
    if (CASE("publication-commands")) {
        for (size_t i = 0U; i < sizeof publications / sizeof publications[0]; ++i) {
            assert(umi_ai_authorengine_plan_publication_invocation(&config, publications[i], NULL, &invocation) == UMI_STATUS_OK);
            assert(invocation.command == publications[i] && invocation.argument_count == 1U);
            assert(strcmp(invocation.arguments[0], publication_names[i]) == 0);
            assert(strcmp(invocation.working_directory, config.workspace) == 0);
        }
    } else if (CASE("ai-missing-input")) {
        for (unsigned i = 0U; i < 4U; ++i) {
            assert(umi_ai_authorengine_plan_invocation(&config, command,
                i == 0U ? NULL : session, i == 1U ? NULL : model,
                i == 2U ? NULL : input, i == 3U ? NULL : output, &invocation) == UMI_STATUS_INVALID_ARGUMENT);
            assert(memcmp(&before, &invocation, sizeof before) == 0);
        }
    } else {
        if (publication)
            status = umi_ai_authorengine_plan_publication_invocation(
                CASE("publication-null-config") ? NULL : &config, command, site,
                CASE("publication-null-output") ? NULL : &invocation);
        else
            status = umi_ai_authorengine_plan_invocation(CASE("ai-null-config") ? NULL : &config,
                command, session, model, input, output, CASE("ai-null-output") ? NULL : &invocation);
        assert(status == expected);
        if (status != UMI_STATUS_OK) {
            assert(memcmp(&before, &invocation, sizeof before) == 0);
        } else {
            assert(strcmp(invocation.executable, config.executable) == 0);
            assert(strcmp(invocation.working_directory, config.workspace) == 0);
            assert(invocation.command == command);
            if (publication) {
                assert(invocation.argument_count == (site == NULL ? 1U : 3U));
                if (CASE("publication-alias"))
                    assert(strcmp(invocation.arguments[2], "saved site folder") == 0);
                if (CASE("publication-site-limit"))
                    assert(strcmp(invocation.arguments[2], exact_argument) == 0);
            } else {
                assert(invocation.argument_count == (command == UMI_AI_AUTHOR_ENGINE_GENERATE ? 13U : 5U));
                assert(strcmp(invocation.arguments[2], config.workspace) == 0);
                assert(strcmp(invocation.arguments[4], config.provider) == 0);
                if (CASE("ai-alias")) {
                    assert(strcmp(invocation.arguments[6], "borrowed session") == 0);
                    assert(strcmp(invocation.arguments[8], "borrowed model") == 0);
                    assert(strcmp(invocation.arguments[10], "borrowed input") == 0);
                    assert(strcmp(invocation.arguments[12], "borrowed output") == 0);
                }
                if (CASE("literal-arguments")) {
                    assert(strcmp(invocation.arguments[10], input) == 0);
                    assert(strcmp(invocation.arguments[12], output) == 0);
                }
                if (CASE("ai-argument-limit"))
                    assert(strcmp(invocation.arguments[12], exact_argument) == 0);
            }
        }
    }
    assert(memcmp(&config_before, &config, sizeof config_before) == 0);
    return 0;
#undef CASE
}
