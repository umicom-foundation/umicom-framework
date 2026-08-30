/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_test.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action test.
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

int test_protocol_action_test(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("test",&k)==UMI_STATUS_OK);TEST_CHECK(k==UMI_HELIX_ACTION_TEST);return 0;}
