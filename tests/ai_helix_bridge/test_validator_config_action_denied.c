/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_validator_config_action_denied.c
 *
 * PURPOSE:
 *   Validate AI/Helix bridge behaviour: validator config action denied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <string.h>

int test_validator_config_action_denied(void){UmiAiHelixBridgeConfig c;UmiAiHelixParsedResponse p;TEST_CHECK(umi_ai_helix_bridge_config_init(&c,"p","m")==UMI_STATUS_OK);c.allowed_actions=umi_ai_helix_action_mask(UMI_HELIX_ACTION_READ);umi_ai_helix_parsed_response_init(&p);p.disposition=UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION;p.action_kind=UMI_HELIX_ACTION_BUILD;strcpy(p.target,".");strcpy(p.summary,"build");strcpy(p.rationale,"required");p.confidence=0.8;TEST_CHECK(umi_ai_helix_proposal_validate(&c,UMI_HELIX_AGENT_ROLE_BUILDER,&p)==UMI_STATUS_PERMISSION_DENIED);return 0;}
