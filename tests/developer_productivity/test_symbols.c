/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_symbols.c
 *
 * PURPOSE:
 *   Verify provider-neutral symbol indexing/search.
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
#include "umicom/developer_productivity/symbols.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperSymbolIndex *index = NULL;
    UmiDeveloperSymbol symbol;
    UmiDeveloperSymbol matches[4];
    size_t count = 0U;

    assert(umi_developer_symbol_index_create(&index) == UMI_STATUS_OK);
    (void)memset(&symbol, 0, sizeof(symbol));
    (void)strcpy(symbol.symbol_id, "main");
    (void)strcpy(symbol.name, "main");
    (void)strcpy(symbol.language_id, "developer.language.c23");
    (void)strcpy(symbol.location.uri, "src/main.c");
    symbol.kind = UMI_DEVELOPER_SYMBOL_FUNCTION;
    symbol.location.line = 1U;

    assert(umi_developer_symbol_index_upsert(
        index, &symbol) == UMI_STATUS_OK);
    assert(umi_developer_symbol_index_search(
        index, "mai", matches, 4U, &count) == UMI_STATUS_OK);
    assert(count == 1U);

    umi_developer_symbol_index_destroy(index);
    return 0;
}
