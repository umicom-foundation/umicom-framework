/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_valid_complete.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser valid complete.
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

int test_parser_valid_complete(void){UmiAiHelixParsedResponse p;const char *t="disposition=complete\naction=none\ntarget=-\nsummary=done\nconfidence=1\nrationale=green";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_OK);TEST_CHECK((uint32_t)p.action_kind==0U);return 0;}
