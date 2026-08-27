/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_disposition_defer.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol disposition defer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_protocol_disposition_defer(void){UmiAiHelixDisposition d=(UmiAiHelixDisposition)0;TEST_CHECK(umi_ai_helix_protocol_parse_disposition("defer",&d)==UMI_STATUS_OK);TEST_CHECK(d==UMI_AI_HELIX_DISPOSITION_DEFER);return 0;}
