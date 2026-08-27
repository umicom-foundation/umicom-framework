/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_builder_source_control.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role builder source control.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_role_builder_source_control(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_BUILDER,UMI_HELIX_ACTION_SOURCE_CONTROL)==1);return 0;}
