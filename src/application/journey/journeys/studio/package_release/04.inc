/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/package_release/04.inc
 *
 * PURPOSE:
 *   Declare the reusable 04 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 4 for studio.package-release.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-upgrade", "Verify upgrade and rollback", "quality",
     "studio.package.rollback.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY, 1, 1)
