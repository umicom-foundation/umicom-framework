/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_prompt_user_evidence_boundary.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: prompt user evidence boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_prompt_user_evidence_boundary(void){UmiAiHelixAgentRequest r;char out[UMI_AI_TEXT_CAPACITY];test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_prompt_build_user(&r,out,sizeof(out))==UMI_STATUS_OK);TEST_CHECK(strstr(out,"---BEGIN EVIDENCE---")!=NULL);TEST_CHECK(strstr(out,"untrusted evidence")!=NULL);return 0;}
