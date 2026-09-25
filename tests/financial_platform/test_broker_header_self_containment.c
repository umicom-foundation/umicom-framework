/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_broker_header_self_containment.c
 *
 * PURPOSE:
 *   Prevent the Batch 09 regression where public broker-connectivity headers
 *   declared UmiStatus while relying on trading/types.h to provide it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/order_journal.h"
#include "umicom/broker_connectivity/pretrade_guard.h"
#include "umicom/broker_connectivity/ibkr_adapter.h"

int main(void)
{
    UmiStatus status = UMI_STATUS_OK;
    UmiBrokerOrderJournal journal = {0};
    UmiBrokerPretradeContext context = {0};
    UmiIbkrAdapterConfig config = {0};

    /* No broker implementation is called: this regression proves that the
     * public declarations themselves are self-contained and compile cleanly. */
    return status == UMI_STATUS_OK &&
           sizeof(journal) > 0U &&
           sizeof(context) > 0U &&
           sizeof(config) > 0U
        ? 0 : 1;
}
