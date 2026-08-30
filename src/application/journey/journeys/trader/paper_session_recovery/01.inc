/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/paper_session_recovery/01.inc
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
/* Acceptance step 1 for trader.paper-session-recovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("connect-paper", "Connect the accepted paper adapter", "account",
     "trader.paper.connect", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
