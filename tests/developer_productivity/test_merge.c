/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_merge.c
 *
 * PURPOSE:
 *   Verify automatic one-sided merge and explicit conflict resolution.
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
#include "umicom/developer_productivity/merge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperMergeDocument *document = NULL;
    char output[4096];

    assert(umi_developer_merge_document_create(
        "one\ntwo\n",
        "ONE\ntwo\n",
        "one\nTWO\n",
        &document) == UMI_STATUS_OK);
    assert(umi_developer_merge_document_conflict_count(document) == 0U);
    assert(umi_developer_merge_document_render(
        document, output, sizeof(output), 0) == UMI_STATUS_OK);
    assert(strstr(output, "ONE") != NULL);
    assert(strstr(output, "TWO") != NULL);
    umi_developer_merge_document_destroy(document);

    assert(umi_developer_merge_document_create(
        "base\n",
        "left\n",
        "right\n",
        &document) == UMI_STATUS_OK);
    assert(umi_developer_merge_document_conflict_count(document) == 1U);
    assert(umi_developer_merge_document_resolve(
        document, 0U, UMI_DEVELOPER_MERGE_USE_LEFT) == UMI_STATUS_OK);
    assert(umi_developer_merge_document_conflict_count(document) == 0U);
    umi_developer_merge_document_destroy(document);
    return 0;
}
