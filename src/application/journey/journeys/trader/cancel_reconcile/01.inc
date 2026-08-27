/* Acceptance step 1 for trader.cancel-reconcile.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("select-order", "Select an open order", "blotter",
     "trader.order.select", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
