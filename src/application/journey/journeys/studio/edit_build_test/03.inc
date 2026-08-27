/* Acceptance step 3 for studio.edit-build-test.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("build-target", "Build the active target", "output",
     "studio.build.run", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
