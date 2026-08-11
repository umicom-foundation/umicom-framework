/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_duplicate.c
 *
 * PURPOSE:
 *   Validate a focused CodeGuard contract so security tooling remains
 *   deterministic during future refactors.
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
#include <assert.h>
#include <stdio.h>
#include "umicom/codeguard/codeguard.h"
int main(void){FILE *f=fopen("cg_a.c","wb");UmiCodeGuardDuplicateSet *s=NULL;UmiCodeGuardResult *r=NULL;assert(f!=NULL);fputs("int x(void){return 1;}\\n",f);fclose(f);f=fopen("cg_b.c","wb");assert(f!=NULL);fputs("int x(void){return 1;}\\n",f);fclose(f);assert(umi_codeguard_duplicate_set_create(&s)==UMI_STATUS_OK);assert(umi_codeguard_result_create(4U,&r)==UMI_STATUS_OK);assert(umi_codeguard_duplicate_set_add(s,"cg_a.c")==UMI_STATUS_OK);assert(umi_codeguard_duplicate_set_add(s,"cg_b.c")==UMI_STATUS_OK);assert(umi_codeguard_duplicate_emit(s,r)==UMI_STATUS_OK);assert(umi_codeguard_result_count(r)==1U);umi_codeguard_duplicate_set_destroy(s);umi_codeguard_result_destroy(r);remove("cg_a.c");remove("cg_b.c");return 0;}
