/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/simulation_startup/05.inc
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
/* Acceptance step 5 for trader.simulation-startup.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-risk", "Verify independent risk readiness", "risk",
     "trader.risk.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
