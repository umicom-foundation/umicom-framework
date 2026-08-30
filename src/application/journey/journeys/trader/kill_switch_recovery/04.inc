/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/kill_switch_recovery/04.inc
 *
 * PURPOSE:
 *   Declare the reusable 04 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 4 for trader.kill-switch-recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("reset-stop", "Reset after operator review", "risk",
     "studio.action.trading.reset-kill-switch", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
