/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/simulation_order/03.inc
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
/* Acceptance step 3 for trader.simulation-order.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("confirm-submit", "Confirm the simulation submission", "order-entry",
     "studio.action.trading.submit-order", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
