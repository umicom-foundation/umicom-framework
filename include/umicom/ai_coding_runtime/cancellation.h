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
/**
 * Represent the ai coding cancellation data shared with callers of this public contract.
 */
typedef struct UmiAiCodingCancellation {
    atomic_int cancelled;
} UmiAiCodingCancellation;
/**
 * Initialise ai coding cancellation from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_cancellation_init(UmiAiCodingCancellation *token);
/**
 * Provide the ai coding cancellation request operation used by this module and its client
 * applications.
 */
void umi_ai_coding_cancellation_request(UmiAiCodingCancellation *token);
/**
 * Provide the ai coding cancellation is requested operation used by this module and its
 * client applications.
 */
int umi_ai_coding_cancellation_is_requested(
    const UmiAiCodingCancellation *token);
#ifdef __cplusplus
}
#endif
#endif
