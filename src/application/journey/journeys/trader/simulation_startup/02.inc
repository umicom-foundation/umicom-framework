/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/simulation_startup/02.inc
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
/* Acceptance step 2 for trader.simulation-startup.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-live-disarmed", "Verify live execution is disarmed", "risk",
     "trader.live.verify-disarmed", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
