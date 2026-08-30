/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/submission_status.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_STATUS_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_STATUS_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return one only for valid forward submission state transitions. */
int umi_reg_submission_status_transition_allowed(UmiRegLifecycleState from, UmiRegLifecycleState to);

#ifdef __cplusplus
}
#endif

#endif
