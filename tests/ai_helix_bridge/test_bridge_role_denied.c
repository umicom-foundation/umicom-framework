/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_bridge_role_denied.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: bridge role denied.
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

int test_bridge_role_denied(void){UmiAiRuntime rt;TestAiProviderState s;UmiAiHelixBridgeConfig c;UmiAiHelixBridge b;UmiAiHelixAgentRequest r;UmiAiHelixAgentResult x;const char *t="disposition=propose_action\naction=filesystem\ntarget=src/a.c\nsummary=edit\nconfidence=0.8\nrationale=needed";TEST_CHECK(test_ai_runtime_prepare(&rt,&s,t)==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model")==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_init(&b,&rt,&c)==UMI_STATUS_OK);test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_DESIGNER);TEST_CHECK(umi_ai_helix_bridge_run(&b,&r,&x)==UMI_STATUS_PERMISSION_DENIED);return 0;}
