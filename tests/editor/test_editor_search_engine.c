/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_search_engine.c
 *
 * PURPOSE:
 *   Implement the test editor search engine behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework literal search tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/editor/search_engine.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char TEXT[] = "editor Editor editorial editor";
    UmiEditorSearchOptions options = {
        UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE, 1, 0, 16U
    };
    UmiEditorSearchResults results;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_search_literal(TEXT, sizeof(TEXT) - 1U,
            "editor", 6U, &options, &results) != UMI_STATUS_OK) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results.count != 3U || results.truncated) return 2;
    options.case_mode = UMI_EDITOR_SEARCH_CASE_SMART;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_search_literal(TEXT, sizeof(TEXT) - 1U,
            "Editor", 6U, &options, &results) != UMI_STATUS_OK ||
        results.count != 1U || results.matches[0].offset != 7U) return 3;
    options.maximum_matches = 1U;
    options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_search_literal(TEXT, sizeof(TEXT) - 1U,
            "editor", 6U, &options, &results) != UMI_STATUS_OK ||
        results.count != 1U || !results.truncated) return 4;
    return EXIT_SUCCESS;
}
