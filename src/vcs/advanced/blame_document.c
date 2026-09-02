/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/blame_document.c
 *
 * PURPOSE:
 *   Maintain bounded line provenance for an opened source document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/blame_document.h"
#include <string.h>
/*
 * Initialise vcs advanced blame document from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_blame_document_init(UmiVcsAdvancedBlameDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return;
    (void)memset(document, 0, sizeof(*document));
    document->struct_size = (uint32_t)sizeof(*document);
    document->api_version = UMI_VCS_ADVANCED_API_VERSION;
    document->revision_number = 1U;
}
/*
 * Provide the vcs advanced blame document set identity operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_blame_document_set_identity(UmiVcsAdvancedBlameDocument *document,
                                                        const char *path,
                                                        const char *revision)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || !umi_vcs_advanced_text_present(path)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(document->path, sizeof(document->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_copy_text(document->revision, sizeof(document->revision), revision);
}
/*
 * Add vcs advanced blame document only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_blame_document_add(UmiVcsAdvancedBlameDocument *document,
                                               const UmiVcsAdvancedBlameLine *line)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || umi_vcs_advanced_blame_line_validate(line) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->line_count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->line_count > 0U &&
        line->line_number <= document->lines[document->line_count - 1U].line_number)
        return UMI_STATUS_INVALID_STATE;
    document->lines[document->line_count++] = *line;
    document->revision_number += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced blame document line operation used by this module and its
 * client applications.
 */
const UmiVcsAdvancedBlameLine *umi_vcs_advanced_blame_document_line(
    const UmiVcsAdvancedBlameDocument *document, size_t line_number)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || line_number == 0U) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < document->line_count; ++i)
        /* Apply this branch only when its contract condition is satisfied. */
        if (document->lines[i].line_number == line_number) return &document->lines[i];
    return NULL;
}
/*
 * Return the number of records represented by vcs advanced blame document distinct commit
 * without changing their state.
 */
size_t umi_vcs_advanced_blame_document_distinct_commit_count(
    const UmiVcsAdvancedBlameDocument *document)
{
    size_t i, j, count = 0U;
    int seen;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < document->line_count; ++i) {
        seen = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < i; ++j)
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (umi_vcs_advanced_text_equal(document->lines[i].commit_oid, document->lines[j].commit_oid)) { seen = 1; break; }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!seen) count += 1U;
    }
    return count;
}
