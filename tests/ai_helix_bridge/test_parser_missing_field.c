/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_missing_field.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser missing field.
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

/*
 * Exercise test parser missing field and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_parser_missing_field(void){UmiAiHelixParsedResponse p;const char *t="disposition=complete\naction=none\ntarget=-\nsummary=done\nconfidence=1";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_PARSE_ERROR);return 0;}
