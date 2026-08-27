/* Acceptance step 1 for trader.replay-strategy.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("load-replay", "Load deterministic replay evidence", "replay",
     "trader.replay.load", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
