/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_rules_temp_file.c
 *
 * PURPOSE:
 *   Validate registration of the temp file CodeGuard rule pack.
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
#include "umicom/codeguard/rules/temp_file.h"
int main(void){size_t count=0U;const UmiCodeGuardRule *rules=umi_codeguard_rules_temp_file(&count);assert(rules!=NULL);assert(count>0U);assert(rules[0].rule_id!=NULL);return 0;}
