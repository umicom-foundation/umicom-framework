/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/cancel_reconcile/01.inc
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
/* Acceptance step 1 for trader.cancel-reconcile.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("select-order", "Select an open order", "blotter",
     "trader.order.select", UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0)
