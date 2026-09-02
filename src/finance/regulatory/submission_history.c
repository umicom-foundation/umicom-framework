/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_history.c
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

#include "umicom/finance/regulatory/submission_history.h"

#include <string.h>
/*
 * Initialise reg submission history from caller-provided values so later operations
 * receive a known state.
 */
void umi_reg_submission_history_init(UmiSubmissionHistory *history){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(history!=NULL)memset(history,0,sizeof *history); }
/*
 * Add reg submission history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_reg_submission_history_append(UmiSubmissionHistory *history,UmiRegLifecycleState state,int64_t event_ms){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(history==NULL||event_ms<0)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(history->count>=UMI_REG_MAX_HISTORY)return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(history->count>0U&&event_ms<history->entries[history->count-1U].event_ms)return UMI_STATUS_INVALID_STATE; history->entries[history->count].state=state; history->entries[history->count].event_ms=event_ms; ++history->count; return UMI_STATUS_OK; }
