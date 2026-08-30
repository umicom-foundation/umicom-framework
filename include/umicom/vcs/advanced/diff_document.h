/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_document.h
 *
 * PURPOSE:
 *   Describe one compared document and aggregate change statistics.
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
#ifndef UMICOM_VCS_ADVANCED_DIFF_DOCUMENT_H
#define UMICOM_VCS_ADVANCED_DIFF_DOCUMENT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedDiffDocument {
    uint32_t struct_size;
    uint32_t api_version;
    char left_path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char right_path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char left_revision[UMI_VCS_ADVANCED_OID_CAPACITY];
    char right_revision[UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t hunk_count;
    size_t added_lines;
    size_t deleted_lines;
    size_t modified_lines;
    int binary;
    UmiVcsCompareMode mode;
} UmiVcsAdvancedDiffDocument;

void umi_vcs_advanced_diff_document_init(UmiVcsAdvancedDiffDocument *value);
UmiStatus umi_vcs_advanced_diff_document_validate(const UmiVcsAdvancedDiffDocument *value);
UmiStatus umi_vcs_advanced_diff_document_set_paths(UmiVcsAdvancedDiffDocument *value,
                                                     const char *left_path,
                                                     const char *right_path,
                                                     UmiVcsCompareMode mode);
size_t umi_vcs_advanced_diff_document_change_count(const UmiVcsAdvancedDiffDocument *value);

#ifdef __cplusplus
}
#endif

#endif
