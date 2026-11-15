/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/profile.h
 *
 * PURPOSE:
 *   Define reusable scan profiles for local development, architecture review
 *   and CI gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_PROFILE_H
#define UMICOM_CODEGUARD_PROFILE_H
#include <stddef.h>
#include "umicom/codeguard/severity.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard profile data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardProfile {
    /* Nonzero flags enable the corresponding source extensions. They apply
     * to direct-file analysis and workspace duplicate collection alike. */
    int scan_c;
    int scan_cpp;
    int scan_headers;
    int scan_architecture;
    int scan_duplicates;
    int scan_source_names;
    /* Include build and _CPack_Packages beneath the selected workspace.
     * Vendor, VCS and codeguard/rules exclusions still apply. This flag does
     * not change the direct-file entry point, which bypasses directory policy. */
    int scan_generated;
    /* Prior reserved-field description retained for review:
     * Reserved input budget: current file readers do not enforce this field.
     * Do not use it as a memory or input-admission security boundary.
     *
     * Native profile-driven reads now enforce this per-file byte budget on
     * each analysis and duplicate pass. Zero admits empty files only.
     * It is not a total workspace, CPU, result-storage or sandbox limit. */
    size_t max_file_bytes;
    size_t max_line_length;
    size_t max_include_count;
    size_t max_source_lines;
    UmiCodeGuardSeverity fail_on;
} UmiCodeGuardProfile;
/**
 * Provide the codeguard profile default operation used by this module and its client
 * applications.
 */
UmiCodeGuardProfile umi_codeguard_profile_default(void);
/**
 * Provide the codeguard profile security operation used by this module and its client
 * applications.
 */
UmiCodeGuardProfile umi_codeguard_profile_security(void);
/**
 * Provide the codeguard profile architecture operation used by this module and its client
 * applications.
 */
UmiCodeGuardProfile umi_codeguard_profile_architecture(void);
/**
 * Provide the codeguard profile ci operation used by this module and its client
 * applications.
 */
UmiCodeGuardProfile umi_codeguard_profile_ci(void);
#ifdef __cplusplus
}
#endif
#endif
