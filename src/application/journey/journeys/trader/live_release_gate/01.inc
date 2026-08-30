/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/live_release_gate/01.inc
 *
 * PURPOSE:
 *   Declare the reusable 01 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 1 for trader.live-release-gate.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-paper-evidence", "Verify accepted paper evidence", "output",
     "trader.live.paper-evidence.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
