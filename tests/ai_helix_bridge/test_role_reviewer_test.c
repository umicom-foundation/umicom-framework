/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_reviewer_test.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role reviewer test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_role_reviewer_test(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_REVIEWER,UMI_HELIX_ACTION_TEST)==1);return 0;}
