/* Acceptance step 2 for trader.kill-switch-recovery.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-rejection", "Verify new orders are rejected", "order-entry",
     "trader.order.rejection.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
