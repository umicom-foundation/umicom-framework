/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/profile.c
 *
 * PURPOSE:
 *   Provide predictable default, security, architecture and CI scan profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/profile.h"
UmiCodeGuardProfile umi_codeguard_profile_default(void){UmiCodeGuardProfile p={1,1,1,1,1,0,4U*1024U*1024U,240U,24U,1600U,UMI_CODEGUARD_HIGH};return p;}
UmiCodeGuardProfile umi_codeguard_profile_security(void){UmiCodeGuardProfile p=umi_codeguard_profile_default();p.max_include_count=32U;p.scan_duplicates=0;return p;}
UmiCodeGuardProfile umi_codeguard_profile_architecture(void){UmiCodeGuardProfile p=umi_codeguard_profile_default();p.fail_on=UMI_CODEGUARD_MEDIUM;p.scan_duplicates=1;return p;}
UmiCodeGuardProfile umi_codeguard_profile_ci(void){UmiCodeGuardProfile p=umi_codeguard_profile_default();p.fail_on=UMI_CODEGUARD_HIGH;p.max_source_lines=1400U;return p;}
