/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_result.c
 *
 * PURPOSE:
 *   Validate a focused CodeGuard contract so security tooling remains
 *   deterministic during future refactors.
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
#include <assert.h>
#include "umicom/codeguard/codeguard.h"
int main(void){UmiCodeGuardResult *r=NULL;UmiCodeGuardFinding f={0};assert(umi_codeguard_result_create(1U,&r)==UMI_STATUS_OK);f.severity=UMI_CODEGUARD_HIGH;f.category=UMI_CODEGUARD_CATEGORY_MEMORY;assert(umi_codeguard_result_add(r,&f)==UMI_STATUS_OK);assert(umi_codeguard_result_count(r)==1U);assert(umi_codeguard_result_count_severity(r,UMI_CODEGUARD_HIGH)==1U);umi_codeguard_result_destroy(r);return 0;}
