/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/cancellation.h
 *
 * PURPOSE:
 *   Provide a small C23 cancellation token shared by long-running context,
 *   provider and validation phases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_CANCELLATION_H
#define UMICOM_AI_CODING_RUNTIME_CANCELLATION_H
#include <stdatomic.h>
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiCodingCancellation {
    atomic_int cancelled;
} UmiAiCodingCancellation;
void umi_ai_coding_cancellation_init(UmiAiCodingCancellation *token);
void umi_ai_coding_cancellation_request(UmiAiCodingCancellation *token);
int umi_ai_coding_cancellation_is_requested(
    const UmiAiCodingCancellation *token);
#ifdef __cplusplus
}
#endif
#endif
