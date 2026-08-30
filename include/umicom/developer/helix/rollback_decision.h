/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/rollback_decision.h
 *
 * PURPOSE:
 *   Evaluate validation, safety, build and test evidence to choose continue, stop or rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ROLLBACK_DECISION_H
#define UMICOM_DEVELOPER_HELIX_ROLLBACK_DECISION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Choose rollback for safety/build/test blockers, review for quality regression, otherwise allow continuation. */
UmiHelixDecision umi_helix_rollback_decision_evaluate(bool safety_blocked,bool build_failed,bool tests_failed,double regression,double max_regression);
#ifdef __cplusplus
}
#endif
#endif
