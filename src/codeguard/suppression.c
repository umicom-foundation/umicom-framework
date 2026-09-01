/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/suppression.c
 *
 * PURPOSE:
 *   Match explicit suppressions by rule identifier and optional path fragment.
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

#include "umicom/codeguard/suppression.h"
#include <string.h>
int umi_codeguard_suppression_matches(const UmiCodeGuardSuppression *s,const UmiCodeGuardFinding *f){if(s==NULL||f==NULL||s->rule_id==NULL)return 0;if(strcmp(s->rule_id,f->rule_id)!=0)return 0;return s->path_substring==NULL||strstr(f->path,s->path_substring)!=NULL;}
