/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_prompt_system_role.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: prompt system role.
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

/*
 * Exercise test prompt system role and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_prompt_system_role(void){char out[UMI_AI_TEXT_CAPACITY];TEST_CHECK(umi_ai_helix_prompt_build_system(UMI_HELIX_AGENT_ROLE_BUILDER,out,sizeof(out))==UMI_STATUS_OK);TEST_CHECK(strstr(out,"Builder Agent")!=NULL);return 0;}
