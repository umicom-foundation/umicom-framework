/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/cancel_reconcile/02.inc
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
/* Acceptance step 2 for trader.cancel-reconcile.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("cancel-order", "Cancel the selected order", "blotter",
     "studio.action.trading.cancel-order", UMI_APPLICATION_JOURNEY_EVIDENCE_SAFETY, 1, 1)
