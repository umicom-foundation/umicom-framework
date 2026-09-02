/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/protocol.h
 *
 * PURPOSE:
 *   Define the small deterministic text protocol used for AI-to-Helix proposals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_PROTOCOL_H
#define INCLUDE_UMICOM_AI_HELIX_PROTOCOL_H

#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai helix protocol specification operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_protocol_specification(void);
/**
 * Provide the ai helix protocol action text operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_protocol_action_text(UmiHelixActionKind kind);
/**
 * Provide the ai helix protocol parse disposition operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_helix_protocol_parse_disposition(
    const char *text,
    UmiAiHelixDisposition *out_disposition);
/**
 * Provide the ai helix protocol parse action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_helix_protocol_parse_action(
    const char *text,
    UmiHelixActionKind *out_kind);

#ifdef __cplusplus
}
#endif

#endif
