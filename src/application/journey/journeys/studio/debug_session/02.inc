/* Acceptance step 2 for studio.debug-session.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 */
STEP("set-breakpoint", "Set a source breakpoint", "editor",
     "studio.debug.breakpoint.add", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
