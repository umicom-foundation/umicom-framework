/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_config_zero_tokens.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: config zero tokens.
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
 * Exercise test config zero tokens and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_config_zero_tokens(void){UmiAiHelixBridgeConfig c;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"p","m")==UMI_STATUS_OK);c.max_output_tokens=0U;TEST_CHECK(umi_ai_helix_bridge_config_validate(&c)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
