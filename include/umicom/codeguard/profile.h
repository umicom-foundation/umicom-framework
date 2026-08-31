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

/* BEGINNER NOTE:
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
typedef struct UmiCodeGuardProfile {
    int scan_c;
    int scan_cpp;
    int scan_headers;
    int scan_architecture;
    int scan_duplicates;
    int scan_source_names;
    int scan_generated;
    size_t max_file_bytes;
    size_t max_line_length;
    size_t max_include_count;
    size_t max_source_lines;
    UmiCodeGuardSeverity fail_on;
} UmiCodeGuardProfile;
UmiCodeGuardProfile umi_codeguard_profile_default(void);
UmiCodeGuardProfile umi_codeguard_profile_security(void);
UmiCodeGuardProfile umi_codeguard_profile_architecture(void);
UmiCodeGuardProfile umi_codeguard_profile_ci(void);
#ifdef __cplusplus
}
#endif
#endif
