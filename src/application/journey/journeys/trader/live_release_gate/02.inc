/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/live_release_gate/02.inc
 *
 * PURPOSE:
 *   Declare the reusable 02 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 2 for trader.live-release-gate.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-risk-independence", "Verify UI cannot bypass risk", "risk",
     "trader.live.risk-independence.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
