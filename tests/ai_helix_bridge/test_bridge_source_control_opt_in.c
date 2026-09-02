/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_bridge_source_control_opt_in.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: bridge source control opt in.
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
 * Exercise test bridge source control opt in and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_bridge_source_control_opt_in(void){UmiAiRuntime rt;TestAiProviderState s;UmiAiHelixBridgeConfig c;UmiAiHelixBridge b;UmiAiHelixAgentRequest r;UmiAiHelixAgentResult x;const char *t="disposition=propose_action\naction=source_control\ntarget=.\nsummary=prepare source-control change\nconfidence=0.7\nrationale=explicitly requested";TEST_CHECK(test_ai_runtime_prepare(&rt,&s,t)==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model")==UMI_STATUS_OK);c.allowed_actions|=umi_ai_helix_action_mask(UMI_HELIX_ACTION_SOURCE_CONTROL);c.allow_source_control=1;TEST_CHECK(umi_ai_helix_bridge_init(&b,&rt,&c)==UMI_STATUS_OK);test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_bridge_run(&b,&r,&x)==UMI_STATUS_OK);TEST_CHECK(x.action.kind==UMI_HELIX_ACTION_SOURCE_CONTROL && x.action.mutating==1);return 0;}
