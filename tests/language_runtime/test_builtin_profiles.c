/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_builtin_profiles.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/builtin_profiles.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){const UmiLanguageServerProfile*p;assert(umi_language_runtime_builtin_profile_count()==15U);p=umi_language_runtime_builtin_profile_for_language("developer.language.php");assert(p&&strcmp(p->id,"language.server.php")==0);return 0;}
