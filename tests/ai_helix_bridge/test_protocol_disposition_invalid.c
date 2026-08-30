/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_disposition_invalid.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol disposition invalid.
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

int test_protocol_disposition_invalid(void){UmiAiHelixDisposition d=(UmiAiHelixDisposition)0;TEST_CHECK(umi_ai_helix_protocol_parse_disposition("invalid",&d)==UMI_STATUS_PARSE_ERROR);return 0;}
