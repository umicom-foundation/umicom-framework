/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_bridge_length_finish.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: bridge length finish.
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
 * Exercise test bridge length finish and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_bridge_length_finish(void){UmiAiRuntime rt;TestAiProviderState s;UmiAiHelixBridgeConfig c;UmiAiHelixBridge b;UmiAiHelixAgentRequest r;UmiAiHelixAgentResult x;const char *t="disposition=complete\naction=none\ntarget=-\nsummary=done\nconfidence=1\nrationale=green";TEST_CHECK(test_ai_runtime_prepare(&rt,&s,t)==UMI_STATUS_OK);s.finish_reason=UMI_AI_FINISH_LENGTH;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"test.helix","model")==UMI_STATUS_OK);TEST_CHECK(umi_ai_helix_bridge_init(&b,&rt,&c)==UMI_STATUS_OK);test_bridge_request(&r,UMI_HELIX_AGENT_ROLE_BUILDER);TEST_CHECK(umi_ai_helix_bridge_run(&b,&r,&x)==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
