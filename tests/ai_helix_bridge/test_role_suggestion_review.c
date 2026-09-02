/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_suggestion_review.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role suggestion review.
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
 * Exercise test role suggestion review and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_role_suggestion_review(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_SUGGESTION,UMI_HELIX_ACTION_REVIEW)==1);return 0;}
