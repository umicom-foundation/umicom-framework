/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/request.h
 *
 * PURPOSE:
 *   Define a bounded provider-neutral chat request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The request owns its messages, making it safe to queue or hand to another thread without relying on caller stack memory.
 */

#ifndef INCLUDE_UMICOM_AI_REQUEST_H
#define INCLUDE_UMICOM_AI_REQUEST_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/message.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiRequest {
    char request_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    UmiAiMessage messages[UMI_AI_MAX_MESSAGES];
    size_t message_count;
    uint32_t max_output_tokens;
    double temperature;
    int allow_tools;
} UmiAiRequest;

void umi_ai_request_init(UmiAiRequest *request);
UmiStatus umi_ai_request_add_message(UmiAiRequest *request,
                                     const UmiAiMessage *message);

#ifdef __cplusplus
}
#endif

#endif
