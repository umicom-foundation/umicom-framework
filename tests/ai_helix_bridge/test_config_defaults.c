/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_config_defaults.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: config defaults.
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
 * Exercise test config defaults and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_config_defaults(void){UmiAiHelixBridgeConfig c;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model") == UMI_STATUS_OK);TEST_CHECK(c.max_output_tokens==768U);TEST_CHECK(c.allow_source_control==0);TEST_CHECK((c.allowed_actions & umi_ai_helix_action_mask(UMI_HELIX_ACTION_SOURCE_CONTROL))==0U);return 0;}
