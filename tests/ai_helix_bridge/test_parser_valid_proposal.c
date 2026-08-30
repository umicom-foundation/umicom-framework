/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_parser_valid_proposal.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: parser valid proposal.
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

int test_parser_valid_proposal(void){UmiAiHelixParsedResponse p;const char *t="disposition=propose_action\naction=filesystem\ntarget=src/a.c\nsummary=edit file\nconfidence=0.75\nrationale=needed";TEST_CHECK(umi_ai_helix_response_parse(t,1,&p)==UMI_STATUS_OK);TEST_CHECK(p.action_kind==UMI_HELIX_ACTION_FILESYSTEM);TEST_CHECK(p.confidence>0.74 && p.confidence<0.76);return 0;}
