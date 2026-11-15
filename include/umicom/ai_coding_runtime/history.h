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
/** Retain a complete group atomically, evicting only the oldest existing records
 * when necessary. snapshot_count may not exceed UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY; a zero count
 * accepts NULL snapshots and changes nothing. Text must be terminated; empty
 * labels and scalar values remain valid evidence. Refusal preserves history.
 * Inputs must be stable and separate from owner storage. Serialize all access. */
UmiStatus UmiAiCodingHistoryRecordBatch(UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshots, size_t snapshot_count);

/** Copy up to capacity records from an oldest-first offset without consuming
 * history. offset equal to count returns OK with zero copied; a larger offset
 * returns NOT_FOUND. NULL snapshots is allowed only for zero capacity. Every
 * failure preserves outputs. out_count is required, separate writable storage;
 * outputs must not overlap history storage. Serialize this read with mutations:
 * offsets may shift after retention evicts older records. */
UmiStatus UmiAiCodingHistoryCopyRange(const UmiAiCodingHistory *history,
    size_t offset, UmiAiCodingAgentSnapshot *snapshots, size_t capacity,
    size_t *out_count);

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
/* Predict the matching batch mutation without changing this owner.
 * A successful check is valid only while the owner and inputs remain unchanged
 * on the serialized owner thread. It reserves no capacity and acquires no lock. */
UmiStatus UmiAiCodingHistoryCheckRecordBatch(const UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshots, size_t snapshot_count);

#ifdef __cplusplus
}
#endif
#endif
