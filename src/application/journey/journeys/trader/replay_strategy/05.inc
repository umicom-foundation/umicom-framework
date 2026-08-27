/* Acceptance step 5 for trader.replay-strategy.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-no-live-route", "Verify replay cannot reach live transport", "risk",
     "trader.replay.live-route.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
