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
#include <stdio.h>
#include <string.h>
#include "umicom/developer_productivity/merge.h"

/* Keep these calls and checks active in Release builds too. */
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperMergeDocument *document = NULL;
    char output[4096];

    CHECK(umi_developer_merge_document_create(
        "one\ntwo\n",
        "ONE\ntwo\n",
        "one\nTWO\n",
        &document) == UMI_STATUS_OK);
    CHECK(umi_developer_merge_document_conflict_count(document) == 0U);
    CHECK(umi_developer_merge_document_render(
        document, output, sizeof(output), 0) == UMI_STATUS_OK);
    CHECK(strstr(output, "ONE") != NULL);
    CHECK(strstr(output, "TWO") != NULL);
    umi_developer_merge_document_destroy(document);

    CHECK(umi_developer_merge_document_create(
        "base\n",
        "left\n",
        "right\n",
        &document) == UMI_STATUS_OK);
    CHECK(umi_developer_merge_document_conflict_count(document) == 1U);
    CHECK(umi_developer_merge_document_resolve(
        document, 0U, UMI_DEVELOPER_MERGE_USE_LEFT) == UMI_STATUS_OK);
    CHECK(umi_developer_merge_document_conflict_count(document) == 0U);
    umi_developer_merge_document_destroy(document);

    /* Every parsing failure must leave a null output and avoid freeing an
     * uninitialised document pointer. Exercise all three input positions. */
    char longLine[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY + 1U];
    char manyLines[(UMI_DEVELOPER_MERGE_MAX_BLOCKS + 1U) * 2U + 1U];
    memset(longLine, 'x', sizeof longLine - 1U);
    longLine[sizeof longLine - 1U] = '\0';
    for (size_t index = 0U; index < sizeof manyLines - 1U; index += 2U) {
        manyLines[index] = 'x';
        manyLines[index + 1U] = '\n';
    }
    manyLines[sizeof manyLines - 1U] = '\0';
    const char *invalidInputs[] = {longLine, manyLines};
    for (size_t input = 0U; input < 2U; ++input) {
        for (size_t side = 0U; side < 3U; ++side) {
            const char *texts[] = {"base", "left", "right"};
            texts[side] = invalidInputs[input];
            document = NULL;
            CHECK(umi_developer_merge_document_create(texts[0], texts[1], texts[2],
                                                       &document) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(document == NULL);
        }
    }
    return 0;
}
