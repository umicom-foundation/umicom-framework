/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_test_build.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role test build.
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

int test_role_test_build(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_TEST,UMI_HELIX_ACTION_BUILD)==1);return 0;}
