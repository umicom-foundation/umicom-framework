/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/package_release/01.inc
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
/* Acceptance step 1 for studio.package-release.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("run-quality-gates", "Run architecture and quality gates", "quality",
     "studio.quality.gates.run", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
