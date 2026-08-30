/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/submission_history.h
 *
 * PURPOSE:
 *   Maintain ordered lifecycle evidence for a regulatory submission.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_HISTORY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_HISTORY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSubmissionHistoryEntry { UmiRegLifecycleState state; int64_t event_ms; } UmiSubmissionHistoryEntry;
typedef struct UmiSubmissionHistory { UmiSubmissionHistoryEntry entries[UMI_REG_MAX_HISTORY]; size_t count; } UmiSubmissionHistory;
/* Initialise an empty submission history. */
void umi_reg_submission_history_init(UmiSubmissionHistory *history);
/* Append a monotonic timestamped state transition. */
UmiStatus umi_reg_submission_history_append(UmiSubmissionHistory *history, UmiRegLifecycleState state, int64_t event_ms);

#ifdef __cplusplus
}
#endif

#endif
