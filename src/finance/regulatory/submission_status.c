/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_status.c
 *
 * PURPOSE:
 *   Enforce monotonic regulatory submission lifecycle transitions.
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

#include "umicom/finance/regulatory/submission_status.h"

int umi_reg_submission_status_transition_allowed(UmiRegLifecycleState from, UmiRegLifecycleState to)
{
    if (from == UMI_REG_STATE_DRAFT) return to == UMI_REG_STATE_VALIDATED || to == UMI_REG_STATE_CANCELLED;
    if (from == UMI_REG_STATE_VALIDATED) return to == UMI_REG_STATE_APPROVED || to == UMI_REG_STATE_DRAFT || to == UMI_REG_STATE_CANCELLED;
    if (from == UMI_REG_STATE_APPROVED) return to == UMI_REG_STATE_SUBMITTED || to == UMI_REG_STATE_CANCELLED;
    if (from == UMI_REG_STATE_SUBMITTED) return to == UMI_REG_STATE_ACCEPTED || to == UMI_REG_STATE_REJECTED;
    if (from == UMI_REG_STATE_REJECTED) return to == UMI_REG_STATE_DRAFT || to == UMI_REG_STATE_CANCELLED;
    return 0;
}
