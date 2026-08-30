/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_prompt_user_objective.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: prompt user objective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_prompt_user_objective(void){UmiAiHelixAgentRequest r;char out[UMI_AI_TEXT_CAPACITY];test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_prompt_build_user(&r,out,sizeof(out))==UMI_STATUS_OK);TEST_CHECK(strstr(out,"Improve the bounded implementation safely")!=NULL);return 0;}
