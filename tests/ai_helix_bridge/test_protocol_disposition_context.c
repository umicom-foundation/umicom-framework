/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_disposition_context.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol disposition context.
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

int test_protocol_disposition_context(void){UmiAiHelixDisposition d=(UmiAiHelixDisposition)0;TEST_CHECK(umi_ai_helix_protocol_parse_disposition("needs_context",&d)==UMI_STATUS_OK);TEST_CHECK(d==UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT);return 0;}
