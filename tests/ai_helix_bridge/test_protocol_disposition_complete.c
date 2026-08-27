/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_disposition_complete.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol disposition complete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_protocol_disposition_complete(void){UmiAiHelixDisposition d=(UmiAiHelixDisposition)0;TEST_CHECK(umi_ai_helix_protocol_parse_disposition("complete",&d)==UMI_STATUS_OK);TEST_CHECK(d==UMI_AI_HELIX_DISPOSITION_COMPLETE);return 0;}
