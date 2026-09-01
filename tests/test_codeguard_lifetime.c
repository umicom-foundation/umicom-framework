/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_lifetime.c
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
int main(void){UmiCodeGuardLifetimeTracker t;UmiCodeGuardResult *r=NULL;assert(umi_codeguard_result_create(8U,&r)==UMI_STATUS_OK);umi_codeguard_lifetime_init(&t);umi_codeguard_lifetime_scan(&t,"x.c",1U,"free(worker);",r);umi_codeguard_lifetime_scan(&t,"x.c",2U,"worker->state = 1;",r);assert(umi_codeguard_result_count(r)==1U);assert(umi_codeguard_result_at(r,0U)->severity==UMI_CODEGUARD_HIGH);umi_codeguard_result_destroy(r);return 0;}
