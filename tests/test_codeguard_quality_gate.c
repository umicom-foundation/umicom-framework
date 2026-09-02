/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_quality_gate.c
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

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include <assert.h>
#include "umicom/codeguard/codeguard.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCodeGuardResult *r=NULL;UmiCodeGuardQualityGate g=umi_codeguard_quality_gate_default();assert(umi_codeguard_result_create(2U,&r)==UMI_STATUS_OK);assert(umi_codeguard_quality_gate_pass(&g,r));umi_codeguard_result_destroy(r);return 0;}
