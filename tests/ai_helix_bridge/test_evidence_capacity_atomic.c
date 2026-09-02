/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_evidence_capacity_atomic.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: evidence capacity atomic.
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
 * Exercise test evidence capacity atomic and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_evidence_capacity_atomic(void){UmiAiHelixEvidenceContext c;char huge[UMI_AI_HELIX_CONTEXT_CAPACITY];size_t before;memset(huge,'x',sizeof(huge)-1U);huge[sizeof(huge)-1U]='\0';umi_ai_helix_evidence_context_init(&c);TEST_CHECK(umi_ai_helix_evidence_context_add(&c,"ok","value")==UMI_STATUS_OK);before=c.length;TEST_CHECK(umi_ai_helix_evidence_context_add(&c,"huge",huge)==UMI_STATUS_CAPACITY_EXCEEDED);TEST_CHECK(c.length==before && c.item_count==1U);return 0;}
