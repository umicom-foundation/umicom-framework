/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/response_parser.h
 *
 * PURPOSE:
 *   Parse model text into data without granting the model authority to execute actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_RESPONSE_PARSER_H
#define INCLUDE_UMICOM_AI_HELIX_RESPONSE_PARSER_H

#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai helix parsed response data shared with callers of this public contract.
 */
typedef struct UmiAiHelixParsedResponse {
    UmiAiHelixDisposition disposition;
    UmiHelixActionKind action_kind;
    char target[UMI_HELIX_TEXT_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    double confidence;
} UmiAiHelixParsedResponse;

/**
 * Initialise ai helix parsed response from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_helix_parsed_response_init(UmiAiHelixParsedResponse *parsed);
/**
 * Read ai helix response into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_ai_helix_response_parse(const char *text,
                                      int strict_protocol,
                                      UmiAiHelixParsedResponse *parsed);

#ifdef __cplusplus
}
#endif

#endif
