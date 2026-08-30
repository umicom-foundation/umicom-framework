/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/studio/source_control_review/05.inc
 *
 * PURPOSE:
 *   Declare the reusable 05 catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Acceptance step 5 for studio.source-control-review.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-quality", "Verify the quality evidence", "quality",
     "studio.quality.refresh", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
