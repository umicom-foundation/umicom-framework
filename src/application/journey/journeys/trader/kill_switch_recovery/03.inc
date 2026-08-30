/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/kill_switch_recovery/03.inc
 *
 * PURPOSE:
 *   Declare the reusable 03 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 3 for trader.kill-switch-recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("record-reason", "Record and review the safety reason", "risk",
     "trader.risk.reason.review", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
