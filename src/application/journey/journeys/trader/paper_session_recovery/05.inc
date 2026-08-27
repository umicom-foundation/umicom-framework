/* Acceptance step 5 for trader.paper-session-recovery.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("verify-restored-state", "Verify restored selections and risk state", "risk",
     "trader.session.restore.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY, 1, 0)
