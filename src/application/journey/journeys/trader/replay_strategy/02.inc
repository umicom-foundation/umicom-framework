/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/replay_strategy/02.inc
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
/* Acceptance step 2 for trader.replay-strategy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("run-strategy", "Run the strategy in simulation", "strategy",
     "trader.strategy.run", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
