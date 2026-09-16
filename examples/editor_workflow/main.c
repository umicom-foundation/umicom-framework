/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/main.c
 * PURPOSE: Preview a literal rename in a small Umicom Notes source document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/search_engine.h"
#include <stdio.h>
#include <string.h>

/* All input is borrowed; replacement is written into separate caller storage.
 * This example previews text only. It does not modify a file on disk. */
int main(void)
{
    const char *source = "int noteCount = 3;\nint total = noteCount;\n";
    const char *needle = "noteCount";
    const char *replacement = "savedCount";
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 1, 0, 0U};
    UmiEditorSearchMatch match;
    char preview[256];
    size_t replacements = 0U;
    UmiStatus status = UmiEditorSearchNavigate(source, strlen(source),
        needle, strlen(needle), &options, 0U, 0, 0, &match, NULL);
    if (status != UMI_STATUS_OK) {
        fputs("The requested name was not found.\n", stderr);
        return 1;
    }
    printf("First name begins at byte %zu.\n", match.offset);
    status = UmiEditorSearchReplaceAll(source, strlen(source), needle,
        strlen(needle), replacement, strlen(replacement), &options,
        preview, sizeof preview, &replacements);
    if (status != UMI_STATUS_OK) {
        fputs("The complete preview could not be prepared.\n", stderr);
        return 1;
    }
    printf("Replaced %zu occurrences. Preview:\n%s", replacements, preview);
    /* A deterministic check also makes the example suitable for CTest. */
    return replacements == 2U && strcmp(preview,
        "int savedCount = 3;\nint total = savedCount;\n") == 0 ? 0 : 1;
}
