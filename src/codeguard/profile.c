/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/profile.c
 *
 * PURPOSE:
 *   Provide predictable default, security, architecture and CI scan profiles.
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

#include "umicom/codeguard/profile.h"

UmiCodeGuardProfile umi_codeguard_profile_default(void)
{
    /* Designated fields make future profile additions explicit and prevent a
     * new option from silently shifting every numeric limit. */
    UmiCodeGuardProfile profile = {
        .scan_c = 1,
        .scan_cpp = 1,
        .scan_headers = 1,
        .scan_architecture = 1,
        .scan_duplicates = 1,
        .scan_source_names = 1,
        .scan_generated = 0,
        .max_file_bytes = 4U * 1024U * 1024U,
        .max_line_length = 240U,
        .max_include_count = 24U,
        .max_source_lines = 1600U,
        .fail_on = UMI_CODEGUARD_HIGH
    };

    return profile;
}

UmiCodeGuardProfile umi_codeguard_profile_security(void)
{
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();

    profile.max_include_count = 32U;
    profile.scan_duplicates = 0;
    return profile;
}

UmiCodeGuardProfile umi_codeguard_profile_architecture(void)
{
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();

    profile.fail_on = UMI_CODEGUARD_MEDIUM;
    profile.scan_duplicates = 1;
    return profile;
}

UmiCodeGuardProfile umi_codeguard_profile_ci(void)
{
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();

    profile.fail_on = UMI_CODEGUARD_HIGH;
    profile.max_source_lines = 1400U;
    return profile;
}
