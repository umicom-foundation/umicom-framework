/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_duplicate_relaxed.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser duplicate relaxed.
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

int test_parser_duplicate_relaxed(void){UmiAiHelixParsedResponse p;const char *t="disposition=complete\ndisposition=defer\naction=none\ntarget=-\nsummary=done\nconfidence=1\nrationale=x";TEST_CHECK(umi_ai_helix_response_parse(t,0,&p)==UMI_STATUS_OK);TEST_CHECK(p.disposition==UMI_AI_HELIX_DISPOSITION_DEFER);return 0;}
