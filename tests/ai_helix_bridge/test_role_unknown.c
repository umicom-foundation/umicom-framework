/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_unknown.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role unknown.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_role_unknown(void){TEST_CHECK(umi_ai_helix_role_allows_action((UmiHelixAgentRole)99,UMI_HELIX_ACTION_READ)==0);return 0;}
