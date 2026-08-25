/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_line.h
 *
 * PURPOSE:
 *   Represent a normalized line in a side-by-side or inline comparison.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_DIFF_LINE_H
#define UMICOM_VCS_ADVANCED_DIFF_LINE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedDiffLine {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsDiffKind kind;
    size_t old_line;
    size_t new_line;
    char text[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    uint64_t fingerprint;
} UmiVcsAdvancedDiffLine;

void umi_vcs_advanced_diff_line_init(UmiVcsAdvancedDiffLine *value);
UmiStatus umi_vcs_advanced_diff_line_validate(const UmiVcsAdvancedDiffLine *value);
UmiStatus umi_vcs_advanced_diff_line_set(UmiVcsAdvancedDiffLine *value,
                                           UmiVcsDiffKind kind,
                                           size_t old_line,
                                           size_t new_line,
                                           const char *text);

#ifdef __cplusplus
}
#endif

#endif
