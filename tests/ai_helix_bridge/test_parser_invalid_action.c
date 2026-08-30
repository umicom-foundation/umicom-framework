/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_invalid_action.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser invalid action.
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

int test_parser_invalid_action(void){UmiAiHelixParsedResponse p;const char *t="disposition=propose_action\naction=root_shell\ntarget=-\nsummary=x\nconfidence=0.5\nrationale=x";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_PARSE_ERROR);return 0;}
