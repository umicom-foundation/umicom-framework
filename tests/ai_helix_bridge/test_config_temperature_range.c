/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_config_temperature_range.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: config temperature range.
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

int test_config_temperature_range(void){UmiAiHelixBridgeConfig c;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"p","m")==UMI_STATUS_OK);c.temperature=2.1;TEST_CHECK(umi_ai_helix_bridge_config_validate(&c)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
