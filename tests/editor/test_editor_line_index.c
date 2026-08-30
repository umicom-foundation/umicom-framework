/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_line_index.c
 *
 * PURPOSE:
 *   Implement the test editor line index behavior for
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
 * Umicom Framework line index tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/editor/line_index.h"

int main(void)
{
    static const char SOURCE[] = "zero\none\ntwo\n";
    UmiEditorLineIndex *index = NULL;
    UmiEditorLineLocation location;
    size_t start;
    size_t end;

    if (umi_editor_line_index_create(&index) != UMI_STATUS_OK) return 1;
    if (umi_editor_line_index_rebuild(index, SOURCE, sizeof(SOURCE) - 1U, 42U) != UMI_STATUS_OK) return 2;
    if (umi_editor_line_index_count(index) != 4U ||
        umi_editor_line_index_source_revision(index) != 42U) return 3;
    if (umi_editor_line_index_locate_offset(index, 7U, &location) != UMI_STATUS_OK ||
        location.line != 1U || location.column_bytes != 2U) return 4;
    if (umi_editor_line_index_line_range(index, 2U, &start, &end) != UMI_STATUS_OK ||
        start != 9U || end != 12U) return 5;
    if (umi_editor_line_index_line_range(index, 9U, &start, &end) != UMI_STATUS_NOT_FOUND) return 6;
    umi_editor_line_index_destroy(index);
    return EXIT_SUCCESS;
}
