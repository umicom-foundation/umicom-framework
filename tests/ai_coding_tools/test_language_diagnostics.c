/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_language_diagnostics.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools language diagnostics.
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
#include "tool_test_support.h"

int main(void)
{
    ToolTestFixture f;
    UmiLanguageService *language = NULL;
    UmiLanguageDiagnosticSnapshot diagnostic = {0};
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(umi_language_service_create(&language) == UMI_STATUS_OK);

    diagnostic.struct_size = (uint32_t)sizeof(diagnostic);
    diagnostic.api_version = UMI_LANGUAGE_DIAGNOSTIC_API_VERSION;
    (void)strcpy(diagnostic.id, "diagnostic.1");
    (void)strcpy(diagnostic.document_id, "file:///src/main.c");
    (void)strcpy(diagnostic.message, "undeclared identifier");
    (void)strcpy(diagnostic.source, "clangd");
    diagnostic.severity = 1;
    diagnostic.line = 5U;

    assert(umi_language_diagnostic_registry_upsert(
        umi_language_service_diagnostic(language), &diagnostic) ==
        UMI_STATUS_OK);
    assert(umi_ai_coding_tool_environment_set_language(
        &f.environment, language) == UMI_STATUS_OK);

    call.call_id = 11U;
    (void)strcpy(call.tool_id, "language.diagnostics");
    (void)strcpy(call.arguments_json, "{}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "undeclared identifier") != NULL);

    umi_language_service_destroy(language);
    tool_test_fixture_deinit(&f);
    return 0;
}

