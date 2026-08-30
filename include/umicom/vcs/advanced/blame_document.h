/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/blame_document.h
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
#ifndef UMICOM_VCS_ADVANCED_BLAME_DOCUMENT_H
#define UMICOM_VCS_ADVANCED_BLAME_DOCUMENT_H
#include "umicom/vcs/advanced/blame_line.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedBlameDocument {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char revision[UMI_VCS_ADVANCED_OID_CAPACITY];
    UmiVcsAdvancedBlameLine lines[UMI_VCS_ADVANCED_LIST_CAPACITY];
    size_t line_count;
    uint64_t revision_number;
} UmiVcsAdvancedBlameDocument;
void umi_vcs_advanced_blame_document_init(UmiVcsAdvancedBlameDocument *document);
UmiStatus umi_vcs_advanced_blame_document_set_identity(UmiVcsAdvancedBlameDocument *document,
                                                        const char *path,
                                                        const char *revision);
UmiStatus umi_vcs_advanced_blame_document_add(UmiVcsAdvancedBlameDocument *document,
                                               const UmiVcsAdvancedBlameLine *line);
const UmiVcsAdvancedBlameLine *umi_vcs_advanced_blame_document_line(
    const UmiVcsAdvancedBlameDocument *document, size_t line_number);
size_t umi_vcs_advanced_blame_document_distinct_commit_count(
    const UmiVcsAdvancedBlameDocument *document);
#ifdef __cplusplus
}
#endif
#endif
