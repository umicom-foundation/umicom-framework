/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_rules_database_boundary.c
 *
 * PURPOSE:
 *   Validate registration of the database boundary CodeGuard rule pack.
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
#include "umicom/codeguard/rules/database_boundary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){size_t count=0U;const UmiCodeGuardRule *rules=umi_codeguard_rules_database_boundary(&count);assert(rules!=NULL);assert(count>0U);assert(rules[0].rule_id!=NULL);return 0;}
