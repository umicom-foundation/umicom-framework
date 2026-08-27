/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_invalid.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action invalid.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_protocol_action_invalid(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("erase_everything",&k)==UMI_STATUS_PARSE_ERROR);return 0;}
