/* Acceptance step 3 for trader.simulation-startup.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("load-watchlist", "Load canonical instruments", "watchlist",
     "trader.watchlist.load", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
