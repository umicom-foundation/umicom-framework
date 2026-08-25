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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/blame_document.h"
#include <string.h>
void umi_vcs_advanced_blame_document_init(UmiVcsAdvancedBlameDocument *document)
{
    if (document == NULL) return;
    (void)memset(document, 0, sizeof(*document));
    document->struct_size = (uint32_t)sizeof(*document);
    document->api_version = UMI_VCS_ADVANCED_API_VERSION;
    document->revision_number = 1U;
}
UmiStatus umi_vcs_advanced_blame_document_set_identity(UmiVcsAdvancedBlameDocument *document,
                                                        const char *path,
                                                        const char *revision)
{
    UmiStatus status;
    if (document == NULL || !umi_vcs_advanced_text_present(path)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(document->path, sizeof(document->path), path);
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_copy_text(document->revision, sizeof(document->revision), revision);
}
UmiStatus umi_vcs_advanced_blame_document_add(UmiVcsAdvancedBlameDocument *document,
                                               const UmiVcsAdvancedBlameLine *line)
{
    if (document == NULL || umi_vcs_advanced_blame_line_validate(line) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (document->line_count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (document->line_count > 0U &&
        line->line_number <= document->lines[document->line_count - 1U].line_number)
        return UMI_STATUS_INVALID_STATE;
    document->lines[document->line_count++] = *line;
    document->revision_number += 1U;
    return UMI_STATUS_OK;
}
const UmiVcsAdvancedBlameLine *umi_vcs_advanced_blame_document_line(
    const UmiVcsAdvancedBlameDocument *document, size_t line_number)
{
    size_t i;
    if (document == NULL || line_number == 0U) return NULL;
    for (i = 0U; i < document->line_count; ++i)
        if (document->lines[i].line_number == line_number) return &document->lines[i];
    return NULL;
}
size_t umi_vcs_advanced_blame_document_distinct_commit_count(
    const UmiVcsAdvancedBlameDocument *document)
{
    size_t i, j, count = 0U;
    int seen;
    if (document == NULL) return 0U;
    for (i = 0U; i < document->line_count; ++i) {
        seen = 0;
        for (j = 0U; j < i; ++j)
            if (umi_vcs_advanced_text_equal(document->lines[i].commit_oid, document->lines[j].commit_oid)) { seen = 1; break; }
        if (!seen) count += 1U;
    }
    return count;
}
