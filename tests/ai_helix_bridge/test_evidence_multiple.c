/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_evidence_multiple.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: evidence multiple.
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

int test_evidence_multiple(void){UmiAiHelixEvidenceContext c;umi_ai_helix_evidence_context_init(&c);TEST_CHECK(umi_ai_helix_evidence_context_add(&c,"build","green")==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_evidence_context_add(&c,"tests","passed")==UMI_STATUS_OK);TEST_CHECK(c.item_count==2U);return 0;}
