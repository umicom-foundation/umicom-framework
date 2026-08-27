/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_role_builder_filesystem.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: role builder filesystem.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_role_builder_filesystem(void){TEST_CHECK(umi_ai_helix_role_allows_action(UMI_HELIX_AGENT_ROLE_BUILDER,UMI_HELIX_ACTION_FILESYSTEM)==1);return 0;}
