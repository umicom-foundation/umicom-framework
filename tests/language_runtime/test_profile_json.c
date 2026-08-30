/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_profile_json.c
 *
 * PURPOSE:
 *   Verify the built-in JSON Language Server profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/language_runtime/profiles/json.h"
int main(void){const UmiLanguageServerProfile*p=umi_language_runtime_profile_json();assert(p!=NULL);assert(p->id[0]);assert(p->executable[0]);assert(p->capabilities!=0U);return 0;}
