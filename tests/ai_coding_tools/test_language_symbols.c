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
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
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


#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    ToolTestFixture f;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    UmiLanguageService *language = NULL;
    UmiLanguageSymbolSnapshot symbol = {0};
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);
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
        &state->f.environment, language) == UMI_STATUS_OK);

    call.call_id = 10U;
    (void)strcpy(call.tool_id, "language.symbols");
    (void)strcpy(call.arguments_json, "{\"query\":\"main\"}");

    assert(umi_ai_coding_tool_execute(
        &state->f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"name\":\"main\"") != NULL);

    umi_language_service_destroy(language);
    tool_test_fixture_deinit(&state->f);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
