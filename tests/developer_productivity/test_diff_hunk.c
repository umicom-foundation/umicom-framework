/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diff_hunk.c
 *
 * PURPOSE:
 *   Verify changed rows are grouped into navigable hunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/diff_hunk.h"

int main(void)
{
    UmiDeveloperDiffDocument *document = NULL;
    UmiDeveloperDiffHunkList hunks;

    assert(umi_developer_diff_document_create(
        "a\nb\nc\nd\ne\n",
        "a\nB\nc\nd\nE\n",
        NULL,
        &document) == UMI_STATUS_OK);
    assert(umi_developer_diff_hunks_build(
        document, 0U, &hunks) == UMI_STATUS_OK);
    assert(hunks.count == 2U);

    umi_developer_diff_document_destroy(document);
    return 0;
}
