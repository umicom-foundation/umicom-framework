/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_confidence_range.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser confidence range.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_parser_confidence_range(void){UmiAiHelixParsedResponse p;const char *t="disposition=complete\naction=none\ntarget=-\nsummary=done\nconfidence=1.2\nrationale=x";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_PARSE_ERROR);return 0;}
