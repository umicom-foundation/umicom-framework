/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/first_run_workspace/01.inc
 *
 * PURPOSE:
 *   Declare the reusable 01 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 1 for studio.first-run-workspace.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("start-clean", "Start Studio with a clean profile", "resource-explorer",
     "studio.application.start", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
