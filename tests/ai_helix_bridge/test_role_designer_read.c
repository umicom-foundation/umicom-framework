/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_designer_read.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role designer read.
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
 * Read test role designer into validated module state and return a status when input
 * cannot be used.
 */
int test_role_designer_read(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_DESIGNER,UMI_HELIX_ACTION_READ)==1);return 0;}
