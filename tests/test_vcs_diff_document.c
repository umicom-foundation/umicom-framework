/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_diff_document.c
 *
 * PURPOSE:
 *   Implement the test vcs diff document behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Unified diff document tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include "umicom/umicom.h"
int main(void)
{
    UmiVcsDiffDocument *document = NULL; UmiVcsDiffDocumentSnapshot snapshot;
    const char *diff = "diff --git a/a.c b/a.c\nindex 111..222 100644\n--- a/a.c\n+++ b/a.c\n@@ -10,2 +10,3 @@\n context\n-old\n+new\n+extra\n";
    assert(umi_vcs_diff_document_create(&document) == UMI_STATUS_OK);
    assert(umi_vcs_diff_document_parse(document, "a.c", 1, diff) == UMI_STATUS_OK);
    assert(umi_vcs_diff_document_snapshot(document, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.staged && snapshot.hunks == 1U && snapshot.additions == 2U && snapshot.deletions == 1U);
    assert(umi_vcs_diff_document_line_at(document, 6U)->kind == UMI_VCS_DIFF_DELETION);
    assert(umi_vcs_diff_document_line_at(document, 7U)->kind == UMI_VCS_DIFF_ADDITION);
    umi_vcs_diff_document_destroy(document); return 0;
}
