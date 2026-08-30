/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/replay_strategy/05.inc
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
/* Acceptance step 5 for trader.replay-strategy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-no-live-route", "Verify replay cannot reach live transport", "risk",
     "trader.replay.live-route.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 0)
