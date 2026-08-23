/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diff.c
 *
 * PURPOSE:
 *   Verify side-by-side diff classifies replacement/insertion rows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/diff.h"

int main(void)
{
    UmiDeveloperDiffDocument *document = NULL;
    UmiDeveloperDiffStats stats;

    assert(umi_developer_diff_document_create(
        "alpha\nbeta\ngamma\n",
        "alpha\nBETA\ngamma\ndelta\n",
        NULL,
        &document) == UMI_STATUS_OK);

    stats = umi_developer_diff_document_stats(document);
    assert(stats.changed_rows >= 2U);
    assert(umi_developer_diff_document_row_count(document) >= 4U);

    umi_developer_diff_document_destroy(document);
    return 0;
}
