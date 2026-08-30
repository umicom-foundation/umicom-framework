/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_none.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action none.
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

int test_protocol_action_none(void){UmiHelixActionKind k=UMI_HELIX_ACTION_READ;TEST_CHECK(umi_ai_helix_protocol_parse_action("none",&k)==UMI_STATUS_OK);TEST_CHECK((uint32_t)k==0U);return 0;}
