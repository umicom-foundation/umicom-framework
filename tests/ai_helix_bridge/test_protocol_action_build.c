/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_build.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action build.
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

int test_protocol_action_build(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("build",&k)==UMI_STATUS_OK);TEST_CHECK(k==UMI_HELIX_ACTION_BUILD);return 0;}
