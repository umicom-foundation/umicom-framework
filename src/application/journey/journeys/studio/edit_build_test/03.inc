/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/edit_build_test/03.inc
 *
 * PURPOSE:
 *   Declare the reusable 03 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 3 for studio.edit-build-test.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("build-target", "Build the active target", "output",
     "studio.build.run", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
