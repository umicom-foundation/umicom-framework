/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_initialize.c
 *
 * PURPOSE:
 *   Verify the bounded initialize decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/decoders/initialize.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Preserve the original smoke assertion for review. The active table keeps
 * that scenario and adds independent literal capability contract cases. */
#if 0
int main(void){UmiLanguageRuntimeInitializeResult x; assert(umi_language_runtime_decode_initialize("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"completionProvider\":{},\"hoverProvider\":true}}}",&x)==UMI_STATUS_OK); assert(x.completion&&x.hover);return 0;}
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static unsigned Flags(const UmiLanguageRuntimeInitializeResult *value)
{
    return (value->text_document_sync ? 1U : 0U) | (value->completion ? 2U : 0U) |
        (value->hover ? 4U : 0U) | (value->signature_help ? 8U : 0U) |
        (value->definition ? 16U : 0U) | (value->references ? 32U : 0U) |
        (value->document_symbols ? 64U : 0U) | (value->workspace_symbols ? 128U : 0U) |
        (value->code_actions ? 256U : 0U) | (value->formatting ? 512U : 0U) |
        (value->rename ? 1024U : 0U) | (value->semantic_tokens ? 2048U : 0U) |
        (value->inlay_hints ? 4096U : 0U) | (value->folding_ranges ? 8192U : 0U);
}

int main(int argc, char **argv)
{
    const char *name = argc > 1 ? argv[1] : "smoke";
    if (argc > 2) return 2;
    static const struct { const char *name, *json; UmiStatus status; unsigned flags; } cases[] = {
        {"smoke", "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"completionProvider\":{},\"hoverProvider\":true}}}", UMI_STATUS_OK, 6U},
        {"empty", "{\"result\":{\"capabilities\":{}}}", UMI_STATUS_OK, 0U},
        {"disabled", "{\"result\":{\"capabilities\":{\"hoverProvider\":false,\"completionProvider\":false,\"textDocumentSync\":false}}}", UMI_STATUS_OK, 0U},
        {"null-disabled", "{\"result\":{\"capabilities\":{\"hoverProvider\":null,\"completionProvider\":null,\"textDocumentSync\":null}}}", UMI_STATUS_OK, 0U},
        {"all-options", "{\"result\":{\"capabilities\":{\"textDocumentSync\":{},\"completionProvider\":{},\"hoverProvider\":{},\"signatureHelpProvider\":{},\"definitionProvider\":{},\"referencesProvider\":{},\"documentSymbolProvider\":{},\"workspaceSymbolProvider\":{},\"codeActionProvider\":{},\"documentFormattingProvider\":{},\"renameProvider\":{},\"semanticTokensProvider\":{},\"inlayHintProvider\":{},\"foldingRangeProvider\":{}}}}", UMI_STATUS_OK, 16383U},
        {"all-booleans", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"definitionProvider\":true,\"referencesProvider\":true,\"documentSymbolProvider\":true,\"workspaceSymbolProvider\":true,\"codeActionProvider\":true,\"documentFormattingProvider\":true,\"renameProvider\":true,\"inlayHintProvider\":true,\"foldingRangeProvider\":true}}}", UMI_STATUS_OK, 14324U},
        {"sync-none", "{\"result\":{\"capabilities\":{\"textDocumentSync\":0}}}", UMI_STATUS_OK, 0U},
        {"sync-full", "{\"result\":{\"capabilities\":{\"textDocumentSync\":1}}}", UMI_STATUS_OK, 1U},
        {"sync-incremental", "{\"result\":{\"capabilities\":{\"textDocumentSync\":2}}}", UMI_STATUS_OK, 1U},
        {"hover-array", "{\"result\":{\"capabilities\":{\"hoverProvider\":[]}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"hover-number", "{\"result\":{\"capabilities\":{\"hoverProvider\":1}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"hover-string", "{\"result\":{\"capabilities\":{\"hoverProvider\":\"true\"}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"completion-true", "{\"result\":{\"capabilities\":{\"completionProvider\":true}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"signature-true", "{\"result\":{\"capabilities\":{\"signatureHelpProvider\":true}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"semantic-true", "{\"result\":{\"capabilities\":{\"semanticTokensProvider\":true}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"sync-true", "{\"result\":{\"capabilities\":{\"textDocumentSync\":true}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"sync-fraction", "{\"result\":{\"capabilities\":{\"textDocumentSync\":1.5}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"sync-negative", "{\"result\":{\"capabilities\":{\"textDocumentSync\":-1}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"sync-unknown", "{\"result\":{\"capabilities\":{\"textDocumentSync\":3}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"sync-overflow", "{\"result\":{\"capabilities\":{\"textDocumentSync\":9223372036854775808}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"duplicate-field", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"hoverProvider\":false}}}", UMI_STATUS_ALREADY_EXISTS, 0U},
        {"escaped-duplicate", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"hover\\u0050rovider\":false}}}", UMI_STATUS_ALREADY_EXISTS, 0U},
        {"duplicate-result", "{\"result\":{\"capabilities\":{}},\"result\":{\"capabilities\":{}}}", UMI_STATUS_ALREADY_EXISTS, 0U},
        {"duplicate-capabilities", "{\"result\":{\"capabilities\":{},\"capabilities\":{}}}", UMI_STATUS_ALREADY_EXISTS, 0U},
        {"cap-array", "{\"result\":{\"capabilities\":[]}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"cap-null", "{\"result\":{\"capabilities\":null}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"result-array", "{\"result\":[]}", UMI_STATUS_PARSE_ERROR, 0U},
        {"missing-result", "{}", UMI_STATUS_PARSE_ERROR, 0U},
        {"missing-capabilities", "{\"result\":{}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"invalid-json", "{\"result\":{\"capabilities\":{},}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"late-invalid", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"foldingRangeProvider\":[]}}}", UMI_STATUS_PARSE_ERROR, 0U},
        {"unknown-extension", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"futureProvider\":{\"data\":[1,2]}}}}", UMI_STATUS_OK, 4U},
        {"unrelated-duplicate", "{\"result\":{\"capabilities\":{\"hoverProvider\":true,\"futureProvider\":1,\"futureProvider\":2}}}", UMI_STATUS_OK, 4U}
    };
    UmiLanguageRuntimeInitializeResult output, sentinel;
    memset(&output, 0x5a, sizeof(output)); memcpy(&sentinel, &output, sizeof(output));
    if (strcmp(name, "arguments") == 0) {
        assert(umi_language_runtime_decode_initialize(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_language_runtime_decode_initialize("{}", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0); return 0;
    }
    if (strcmp(name, "byte-limit") == 0) {
        char *large = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY + 1U); assert(large != NULL);
        memset(large, ' ', UMI_LANGUAGE_RUNTIME_JSON_CAPACITY); large[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY] = '\0';
        UmiStatus status = umi_language_runtime_decode_initialize(large, &output); free(large);
        assert(status == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0); return 0;
    }
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(cases[i].name, name) != 0) continue;
        UmiStatus status = umi_language_runtime_decode_initialize(cases[i].json, &output);
        if (status != cases[i].status) fprintf(stderr, "%s: status %d expected %d\n", name, (int)status, (int)cases[i].status);
        assert(status == cases[i].status);
        if (status == UMI_STATUS_OK) assert(Flags(&output) == cases[i].flags);
        else assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        return 0;
    }
    return 2;
}
