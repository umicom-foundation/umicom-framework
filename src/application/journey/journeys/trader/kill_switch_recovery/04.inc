/* Acceptance step 4 for trader.kill-switch-recovery.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("reset-stop", "Reset after operator review", "risk",
     "studio.action.trading.reset-kill-switch", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
