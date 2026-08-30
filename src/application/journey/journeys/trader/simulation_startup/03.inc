/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/simulation_startup/03.inc
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
/* Acceptance step 3 for trader.simulation-startup.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("load-watchlist", "Load canonical instruments", "watchlist",
     "trader.watchlist.load", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
