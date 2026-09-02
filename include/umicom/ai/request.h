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

/*
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

/**
 * Represent the ai request data shared with callers of this public contract.
 */
typedef struct UmiAiRequest {
    char request_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    UmiAiMessage messages[UMI_AI_MAX_MESSAGES];
    size_t message_count;
    uint32_t max_output_tokens;
    double temperature;
    int allow_tools;
} UmiAiRequest;

/**
 * Initialise ai request from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_request_init(UmiAiRequest *request);
/**
 * Provide the ai request add message operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_request_add_message(UmiAiRequest *request,
                                     const UmiAiMessage *message);

#ifdef __cplusplus
}
#endif

#endif
