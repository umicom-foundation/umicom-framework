/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/simulation_startup/04.inc
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
/* Acceptance step 4 for trader.simulation-startup.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("refresh-market", "Refresh simulated market state", "chart",
     "studio.action.trading.refresh", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
