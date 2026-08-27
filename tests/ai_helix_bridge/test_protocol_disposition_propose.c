/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_disposition_propose.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol disposition propose.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_protocol_disposition_propose(void){UmiAiHelixDisposition d=(UmiAiHelixDisposition)0;TEST_CHECK(umi_ai_helix_protocol_parse_disposition("propose_action",&d)==UMI_STATUS_OK);TEST_CHECK(d==UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION);return 0;}
