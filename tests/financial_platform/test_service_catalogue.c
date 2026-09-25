/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify the complete shared financial-platform policy catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/service.h"

int main(void)
{
    UmiFinancialPlatformInput input;
    UmiFinancialPlatformSnapshot snapshot;
    size_t index;

    umi_financial_platform_input_init(&input);
    input.value = 100.0;
    input.referenceValue = 100.0;
    input.limitValue = 1000.0;
    input.tolerance = 1.0;
    input.amount = 100.0;
    input.debitTotal = 1000.0;
    input.creditTotal = 1000.0;
    input.grossExposure = 500.0;
    input.netExposure = 250.0;
    input.pnl = 25.0;
    input.totalAmount = 1000.0;
    input.matchedCount = 10U;
    input.totalCount = 10U;
    input.completedCount = 10U;
    input.auditCount = 10U;
    input.persistedCount = 10U;
    input.ageMilliseconds = 100U;
    input.nowMilliseconds = 1000U;
    input.dueMilliseconds = 2000U;
    input.validIdentity = 1;
    input.active = 1;
    input.trusted = 1;
    input.approved = 1;
    input.currencyReady = 1;
    input.partyReady = 1;
    input.accountReady = 1;
    input.instrumentReady = 1;
    input.tradeReady = 1;
    input.positionReady = 1;
    input.paymentReady = 1;
    input.ledgerReady = 1;
    input.settlementReady = 1;
    input.reconciliationReady = 1;
    input.workflowReady = 1;
    input.auditReady = 1;
    input.transitionAllowed = 1;
    input.rollbackAvailable = 1;
    input.revision = 77U;

    assert(umi_financial_platform_service_count() == 96U);
    for (index = 0U; index < umi_financial_platform_service_count(); ++index) {
        const char *id = NULL;
        assert(umi_financial_platform_service_id_at(index, &id) ==
               UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(umi_financial_platform_service_evaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 77U);
    }

    assert(umi_financial_platform_service_evaluate(
               "missing.capability", &input, &snapshot) ==
           UMI_STATUS_NOT_FOUND);
    return 0;
}
