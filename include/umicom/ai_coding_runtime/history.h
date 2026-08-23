/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/history.h
 *
 * PURPOSE:
 *   Retain bounded agent snapshots for audit, UI history and deterministic tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_HISTORY_H
#define UMICOM_AI_CODING_RUNTIME_HISTORY_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiCodingHistory UmiAiCodingHistory;
UmiStatus umi_ai_coding_history_create(UmiAiCodingHistory **out_history);
void umi_ai_coding_history_destroy(UmiAiCodingHistory *history);
UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot);
UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot);
size_t umi_ai_coding_history_count(const UmiAiCodingHistory *history);
#ifdef __cplusplus
}
#endif
#endif
