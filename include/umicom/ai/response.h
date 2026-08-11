/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/response.h
 *
 * PURPOSE:
 *   Define a bounded provider-neutral model response and token usage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The response keeps generated text, finish reason and usage together so callers can display or persist one self-contained result.
 */

#ifndef INCLUDE_UMICOM_AI_RESPONSE_H
#define INCLUDE_UMICOM_AI_RESPONSE_H

#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiResponse {
    char request_id[UMI_AI_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_TEXT_CAPACITY * 2U];
    UmiAiFinishReason finish_reason;
    UmiAiUsage usage;
} UmiAiResponse;

void umi_ai_response_init(UmiAiResponse *response);
UmiStatus umi_ai_response_set_text(UmiAiResponse *response, const char *text);

#ifdef __cplusplus
}
#endif

#endif
