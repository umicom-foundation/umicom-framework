/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/kill_switch_recovery/05.inc
 *
 * PURPOSE:
 *   Declare the reusable 05 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 5 for trader.kill-switch-recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-recovery", "Verify controlled simulation recovery", "account",
     "trader.recovery.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY, 1, 0)
