/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_bridge_proposal.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: bridge proposal.
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

int test_bridge_proposal(void){UmiAiRuntime rt;TestAiProviderState s;UmiAiHelixBridgeConfig c;UmiAiHelixBridge b;UmiAiHelixAgentRequest r;UmiAiHelixAgentResult x;const char *t="disposition=propose_action\naction=filesystem\ntarget=src/example.c\nsummary=edit bounded implementation\nconfidence=0.9\nrationale=required";TEST_CHECK(test_ai_runtime_prepare(&rt,&s,t)==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model")==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_init(&b,&rt,&c)==UMI_STATUS_OK);test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_bridge_run(&b,&r,&x)==UMI_STATUS_OK);TEST_CHECK(x.disposition==UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION);TEST_CHECK(x.action.kind==UMI_HELIX_ACTION_FILESYSTEM && x.action.mutating==1);return 0;}
