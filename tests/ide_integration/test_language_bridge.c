/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_language_bridge.c
 *
 * PURPOSE:
 *   Integration regression coverage for language bridge.
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
#include "umicom/ide_integration/language_bridge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageService *language = NULL;
    UmiLanguageSymbolSnapshot symbol = {0};
    UmiIdeNavigationTarget target;
    assert(umi_language_service_create(&language) == UMI_STATUS_OK);
    symbol.struct_size = (uint32_t)sizeof(symbol);
    symbol.api_version = UMI_LANGUAGE_SYMBOL_API_VERSION;
    (void)strcpy(symbol.id, "symbol.main");
    (void)strcpy(symbol.document_id, "file:///tmp/main.c");
    (void)strcpy(symbol.name, "main");
    (void)strcpy(symbol.kind, "function");
    symbol.line = 3U;
    assert(umi_language_symbol_registry_upsert(
        umi_language_service_symbol(language), &symbol) == UMI_STATUS_OK);
    assert(umi_ide_language_symbol_target(
        language, "symbol.main", &target) == UMI_STATUS_OK);
    assert(target.location.line == 3U);
    umi_language_service_destroy(language);
    return 0;
}

