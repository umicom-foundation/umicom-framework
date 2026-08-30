/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_report.c
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
#include <stdio.h>
#include "umicom/codeguard/codeguard.h"
int main(void){UmiCodeGuardResult *r=NULL;FILE *f;assert(umi_codeguard_result_create(2U,&r)==UMI_STATUS_OK);f=tmpfile();assert(f!=NULL);assert(umi_codeguard_report_write(f,UMI_CODEGUARD_REPORT_JSON,r)==UMI_STATUS_OK);fclose(f);umi_codeguard_result_destroy(r);return 0;}
