/* Acceptance step 5 for trader.simulation-order.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-position", "Verify positions and P&L reconciliation", "positions",
     "trader.position.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
