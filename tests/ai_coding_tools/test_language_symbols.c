/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_language_symbols.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools language symbols.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    ToolTestFixture f;
    UmiLanguageService *language = NULL;
    UmiLanguageSymbolSnapshot symbol = {0};
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(umi_language_service_create(&language) == UMI_STATUS_OK);

    symbol.struct_size = (uint32_t)sizeof(symbol);
    symbol.api_version = UMI_LANGUAGE_SYMBOL_API_VERSION;
    (void)strcpy(symbol.id, "symbol.main");
    (void)strcpy(symbol.document_id, "file:///src/main.c");
    (void)strcpy(symbol.name, "main");
    (void)strcpy(symbol.kind, "function");
    symbol.line = 10U;

    assert(umi_language_symbol_registry_upsert(
        umi_language_service_symbol(language), &symbol) == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_environment_set_language(
        &f.environment, language) == UMI_STATUS_OK);

    call.call_id = 10U;
    (void)strcpy(call.tool_id, "language.symbols");
    (void)strcpy(call.arguments_json, "{\"query\":\"main\"}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"name\":\"main\"") != NULL);

    umi_language_service_destroy(language);
    tool_test_fixture_deinit(&f);
    return 0;
}

