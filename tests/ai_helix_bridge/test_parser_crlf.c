/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_crlf.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser crlf.
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

int test_parser_crlf(void){UmiAiHelixParsedResponse p;const char *t="disposition=complete\r\naction=none\r\ntarget=-\r\nsummary=done\r\nconfidence=1\r\nrationale=x\r\n";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_OK);return 0;}
