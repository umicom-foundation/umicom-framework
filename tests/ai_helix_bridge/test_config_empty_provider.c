/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_config_empty_provider.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: config empty provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_config_empty_provider(void){UmiAiHelixBridgeConfig c;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"","model") == UMI_STATUS_INVALID_ARGUMENT);return 0;}
