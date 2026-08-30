/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/journeys/trader/cancel_reconcile/03.inc
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
/* Acceptance step 3 for trader.cancel-reconcile.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 */
STEP("verify-terminal-state", "Verify terminal order state", "blotter",
     "trader.order.terminal.verify", UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0)
