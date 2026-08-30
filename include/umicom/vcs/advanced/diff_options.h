/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_options.h
 *
 * PURPOSE:
 *   Define deterministic user comparison options shared by every frontend.
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
#ifndef UMICOM_VCS_ADVANCED_DIFF_OPTIONS_H
#define UMICOM_VCS_ADVANCED_DIFF_OPTIONS_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedDiffOptions {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsWhitespaceMode whitespace;
    size_t context_lines;
    int ignore_case;
    int detect_moves;
    int semantic;
    int word_diff;
    int treat_crlf_as_lf;
} UmiVcsAdvancedDiffOptions;

void umi_vcs_advanced_diff_options_init(UmiVcsAdvancedDiffOptions *value);
UmiStatus umi_vcs_advanced_diff_options_validate(const UmiVcsAdvancedDiffOptions *value);
uint64_t umi_vcs_advanced_diff_options_fingerprint(const UmiVcsAdvancedDiffOptions *value);

#ifdef __cplusplus
}
#endif

#endif
