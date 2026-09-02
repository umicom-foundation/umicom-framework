/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_unknown.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role unknown.
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
 * Exercise test role unknown and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_role_unknown(void){TEST_CHECK(umi_ai_helix_role_allows_action((UmiHelixAgentRole)99,UMI_HELIX_ACTION_READ)==0);return 0;}
