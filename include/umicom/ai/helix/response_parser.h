/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/response_parser.h
 *
 * PURPOSE:
 *   Parse model text into data without granting the model authority to execute actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_RESPONSE_PARSER_H
#define INCLUDE_UMICOM_AI_HELIX_RESPONSE_PARSER_H

#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiHelixParsedResponse {
    UmiAiHelixDisposition disposition;
    UmiHelixActionKind action_kind;
    char target[UMI_HELIX_TEXT_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    double confidence;
} UmiAiHelixParsedResponse;

void umi_ai_helix_parsed_response_init(UmiAiHelixParsedResponse *parsed);
UmiStatus umi_ai_helix_response_parse(const char *text,
                                      int strict_protocol,
                                      UmiAiHelixParsedResponse *parsed);

#ifdef __cplusplus
}
#endif

#endif
