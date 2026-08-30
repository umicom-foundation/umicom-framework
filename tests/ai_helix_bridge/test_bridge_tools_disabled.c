/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_bridge_tools_disabled.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: bridge tools disabled.
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

int test_bridge_tools_disabled(void){UmiAiRuntime rt;TestAiProviderState s;UmiAiHelixBridgeConfig c;UmiAiHelixBridge b;UmiAiHelixAgentRequest r;UmiAiHelixAgentResult x;const char *t="disposition=complete\naction=none\ntarget=-\nsummary=done\nconfidence=1\nrationale=green";TEST_CHECK(test_ai_runtime_prepare(&rt,&s,t)==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model-x")==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_init(&b,&rt,&c)==UMI_STATUS_OK);test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_bridge_run(&b,&r,&x)==UMI_STATUS_OK);TEST_CHECK(s.last_request.allow_tools==0);TEST_CHECK(strcmp(s.last_request.model_id,"model-x")==0);return 0;}
