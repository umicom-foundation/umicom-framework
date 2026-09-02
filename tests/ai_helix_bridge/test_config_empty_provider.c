/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_config_empty_provider.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: config empty provider.
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
 * Exercise test config empty provider and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_config_empty_provider(void){UmiAiHelixBridgeConfig c;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"","model") == UMI_STATUS_INVALID_ARGUMENT);return 0;}
