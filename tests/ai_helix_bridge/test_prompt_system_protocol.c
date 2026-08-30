/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_prompt_system_protocol.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: prompt system protocol.
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

int test_prompt_system_protocol(void){char out[UMI_AI_TEXT_CAPACITY];TEST_CHECK(umi_ai_helix_prompt_build_system(UMI_HELIX_AGENT_ROLE_REVIEWER,out,sizeof(out))==UMI_STATUS_OK);TEST_CHECK(strstr(out,"disposition=<propose_action")!=NULL);TEST_CHECK(strstr(out,"never claim")!=NULL);return 0;}
