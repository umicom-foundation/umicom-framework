/* Acceptance step 1 for trader.kill-switch-recovery.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("engage-stop", "Engage the kill switch", "risk",
     "studio.action.trading.engage-kill-switch", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
