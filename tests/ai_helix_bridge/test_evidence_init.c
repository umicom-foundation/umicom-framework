/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_evidence_init.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: evidence init.
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
 * Initialise test evidence from caller-provided values so later operations receive a known
 * state.
 */
int test_evidence_init(void){UmiAiHelixEvidenceContext c;memset(&c,0x7f,sizeof(c));umi_ai_helix_evidence_context_init(&c);TEST_CHECK(c.length==0U && c.item_count==0U && c.text[0]=='\0');return 0;}
