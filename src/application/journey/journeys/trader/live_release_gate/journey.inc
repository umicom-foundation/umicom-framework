/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/live_release_gate/journey.inc
 *
 * PURPOSE:
 *   Declare the reusable journey catalogue data for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Executable acceptance journey: Guarded live release gate.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
JOURNEY(TRADER_LIVE_RELEASE_GATE, "org.umicom.trader",
        "trader.live-release-gate", "Guarded live release gate", "trading",
        TRADER_LIVE_RELEASE_GATE_STEPS, 1)
