/* Acceptance step 1 for studio.first-run-workspace.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("start-clean", "Start Studio with a clean profile", "resource-explorer",
     "studio.application.start", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
