/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/history.h
 *
 * PURPOSE:
 *   Retain bounded agent snapshots for audit, UI history and deterministic tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_HISTORY_H
#define UMICOM_AI_CODING_RUNTIME_HISTORY_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ai coding history data shared with callers of this public contract.
 */
typedef struct UmiAiCodingHistory UmiAiCodingHistory;
/**
 * Initialise ai coding history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_history_create(UmiAiCodingHistory **out_history);
/**
 * Release or reset state held by ai coding history so the same storage can be reused
 * safely.
 */
void umi_ai_coding_history_destroy(UmiAiCodingHistory *history);
/**
 * Provide the ai coding history record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot);
/**
 * Find ai coding history while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot);
/**
 * Return the number of records represented by ai coding history without changing their
 * state.
 */
size_t umi_ai_coding_history_count(const UmiAiCodingHistory *history);
#ifdef __cplusplus
}
#endif
#endif
