/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_bookmark.h
 *
 * PURPOSE:
 *   Persist named source locations inside long comparison sessions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMPARE_BOOKMARK_H
#define UMICOM_VCS_ADVANCED_COMPARE_BOOKMARK_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedCompareBookmark {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char label[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    size_t line_number;
    size_t change_index;
    uint64_t fingerprint;
} UmiVcsAdvancedCompareBookmark;

void umi_vcs_advanced_compare_bookmark_init(UmiVcsAdvancedCompareBookmark *value);
UmiStatus umi_vcs_advanced_compare_bookmark_validate(const UmiVcsAdvancedCompareBookmark *value);
UmiStatus umi_vcs_advanced_compare_bookmark_set(UmiVcsAdvancedCompareBookmark *value,
                                                  const char *path,
                                                  const char *label,
                                                  size_t line_number,
                                                  size_t change_index);

#ifdef __cplusplus
}
#endif

#endif
