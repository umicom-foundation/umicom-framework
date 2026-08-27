/* Acceptance step 5 for trader.cancel-reconcile.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-audit", "Verify immutable execution evidence", "output",
     "trader.execution.audit.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
