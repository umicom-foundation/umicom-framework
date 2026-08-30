/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/package_release/02.inc
 *
 * PURPOSE:
 *   Declare the reusable 02 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 2 for studio.package-release.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("build-package", "Build the installable package", "output",
     "studio.package.build", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
