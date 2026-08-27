/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/proposal_validator.h
 *
 * PURPOSE:
 *   Validate parsed model proposals against Helix role and bridge action limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_PROPOSAL_VALIDATOR_H
#define INCLUDE_UMICOM_AI_HELIX_PROPOSAL_VALIDATOR_H

#include "umicom/ai/helix/bridge_config.h"
#include "umicom/ai/helix/response_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_helix_proposal_validate(
    const UmiAiHelixBridgeConfig *config,
    UmiHelixAgentRole role,
    const UmiAiHelixParsedResponse *parsed);

#ifdef __cplusplus
}
#endif

#endif
