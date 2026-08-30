/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/paper_session_recovery/04.inc
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
/* Acceptance step 4 for trader.paper-session-recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("restart-session", "Restart the workstation session", "context-inspector",
     "trader.session.restart", UMI_APPLICATION_JOURNEY_EVIDENCE_RECOVERY, 1, 1)
