/* Acceptance step 3 for trader.paper-session-recovery.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("reconcile-orders", "Reconcile paper orders and fills", "blotter",
     "trader.paper.reconcile", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
