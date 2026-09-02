/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_protocol_action_review.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: protocol action review.
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
 * Exercise test protocol action review and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_protocol_action_review(void){UmiHelixActionKind k=(UmiHelixActionKind)0;TEST_CHECK(umi_ai_helix_protocol_parse_action("review",&k)==UMI_STATUS_OK);TEST_CHECK(k==UMI_HELIX_ACTION_REVIEW);return 0;}
