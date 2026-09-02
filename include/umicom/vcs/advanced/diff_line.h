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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_DIFF_LINE_H
#define UMICOM_VCS_ADVANCED_DIFF_LINE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced diff line data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedDiffLine {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsDiffKind kind;
    size_t old_line;
    size_t new_line;
    char text[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    uint64_t fingerprint;
} UmiVcsAdvancedDiffLine;

/**
 * Initialise vcs advanced diff line from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_line_init(UmiVcsAdvancedDiffLine *value);
/**
 * Check that vcs advanced diff line satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_diff_line_validate(const UmiVcsAdvancedDiffLine *value);
/**
 * Copy vcs advanced diff line into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_vcs_advanced_diff_line_set(UmiVcsAdvancedDiffLine *value,
                                           UmiVcsDiffKind kind,
                                           size_t old_line,
                                           size_t new_line,
                                           const char *text);

#ifdef __cplusplus
}
#endif

#endif
