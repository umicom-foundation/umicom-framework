/* Acceptance step 3 for trader.cancel-reconcile.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-terminal-state", "Verify terminal order state", "blotter",
     "trader.order.terminal.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
