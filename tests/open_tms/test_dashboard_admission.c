/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_dashboard_admission.c
 *
 * PURPOSE:
 *   Verify complete dashboard admission and unchanged state on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "operational_fixture.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    /* The service owns several large fixed arrays; heap storage avoids imposing
     * a large automatic stack requirement on the qualification process. */
    UmiOpenTmsService *service = malloc(sizeof(*service));
    REQUIRE(service != NULL);
    umi_open_tms_service_init(service);
    UmiOpenTmsServiceSnapshot result, before;
    memset(&result, 0xa5, sizeof result); memcpy(&before, &result, sizeof result);
    if (strcmp(test, "empty") == 0) {
        UmiStatus status = umi_open_tms_service_snapshot(service, 0, &result);
        free(service);
        REQUIRE(status == UMI_STATUS_OK && result.cashCount == 0 && !result.dashboard.operationsReady);
        REQUIRE(result.dashboard.grossExposure == 0 && result.dashboard.cashHeadroom == 0);
        return 0;
    }
    UmiOpenTmsCashRecord cash = CashRow();
    UmiOpenTmsLiquidityBucket liquidity = LiquidityRow();
    UmiOpenTmsPosition position = PositionRow();
    UmiOpenTmsRiskLimit risk = RiskRow();
    UmiOpenTmsSettlementRecord settlement = SettlementRow();
    UmiOpenTmsReconciliationItem reconciliation = ReconciliationRow();
    UmiOpenTmsAccountingLine debit = {"asset", "GBP", "practice", 10, 0};
    UmiOpenTmsAccountingLine credit = {"control", "GBP", "practice", 0, 10};
    REQUIRE(umi_open_tms_cash_book_upsert(&service->cash, &cash) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_liquidity_ladder_add(&service->liquidity, &liquidity) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_position_book_upsert(&service->positions, &position) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_risk_book_upsert(&service->risk, &risk) == UMI_STATUS_OK);
    settlement.state = UMI_OPEN_TMS_SETTLEMENT_SETTLED;
    REQUIRE(umi_open_tms_settlement_book_add(&service->settlements, &settlement) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_reconciliation_book_add(&service->reconciliation, &reconciliation) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_reconciliation_book_resolve(&service->reconciliation, "match") == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_add(&service->accounting, &debit) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_add(&service->accounting, &credit) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_approve(&service->accounting) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_post(&service->accounting) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0 || strcmp(test, "forward-revision") == 0) {
        if (strcmp(test, "forward-revision") == 0) service->forwards.revision = 100;
        UmiStatus status = umi_open_tms_service_snapshot(service, 1001, &result);
        free(service);
        REQUIRE(status == UMI_STATUS_OK && result.dashboard.operationsReady);
        REQUIRE(result.dashboard.cashHeadroom == 450 && result.dashboard.liquidityGap == -30);
        REQUIRE(result.dashboard.grossExposure == 200 && result.dashboard.unrealisedPnl == 20);
        if (strcmp(test, "forward-revision") == 0) REQUIRE(result.revision == 100);
        return 0;
    }
    if (strcmp(test, "cash-count") == 0) service->cash.count = SIZE_MAX;
    else if (strcmp(test, "liquidity-count") == 0) service->liquidity.count = SIZE_MAX;
    else if (strcmp(test, "position-count") == 0) service->positions.count = SIZE_MAX;
    else if (strcmp(test, "risk-count") == 0) service->risk.count = SIZE_MAX;
    else if (strcmp(test, "settlement-count") == 0) service->settlements.count = SIZE_MAX;
    else if (strcmp(test, "accounting-count") == 0) service->accounting.count = SIZE_MAX;
    else if (strcmp(test, "reconciliation-count") == 0) service->reconciliation.count = SIZE_MAX;
    else if (strcmp(test, "cash-overflow") == 0) { service->cash.records[0].projected = DBL_MAX; service->cash.records[0].inflows = DBL_MAX; }
    else if (strcmp(test, "position-overflow") == 0) service->positions.positions[0].marketPrice = DBL_MAX;
    else if (strcmp(test, "risk-nan") == 0) service->risk.limits[0].limit = NAN;
    else if (strcmp(test, "accounting-balance") == 0) service->accounting.lines[1].credit = 9;
    else if (strcmp(test, "reconciliation-counters") == 0) service->reconciliation.breakCount = 1;
    else if (strcmp(test, "forward-count") == 0) service->forwards.count = SIZE_MAX;
    else if (strcmp(test, "service-revision") == 0) service->revision = 0;
    else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_open_tms_service_snapshot(service, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_open_tms_service_snapshot(NULL, 0, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &result, sizeof result) == 0);
        UmiOpenTmsDashboardSnapshot direct, saved;
        memset(&direct, 0x5a, sizeof direct); memcpy(&saved, &direct, sizeof direct);
        REQUIRE(umi_open_tms_dashboard_snapshot(&service->cash, &service->liquidity,
            &service->positions, &service->risk, &service->settlements, &service->accounting,
            NULL, 0, &direct) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&direct, &saved, sizeof direct) == 0);
        free(service); return 0;
    } else { free(service); return 2; }
    UmiStatus status = umi_open_tms_service_snapshot(service, 1001, &result);
    if (strcmp(test, "forward-count") != 0 && strcmp(test, "service-revision") != 0) {
        UmiOpenTmsDashboardSnapshot direct, saved;
        memset(&direct, 0x5a, sizeof direct); memcpy(&saved, &direct, sizeof direct);
        UmiStatus directStatus = umi_open_tms_dashboard_snapshot(&service->cash, &service->liquidity,
            &service->positions, &service->risk, &service->settlements, &service->accounting,
            &service->reconciliation, 1001, &direct);
        REQUIRE(directStatus != UMI_STATUS_OK && memcmp(&direct, &saved, sizeof direct) == 0);
    }
    free(service);
    REQUIRE(status != UMI_STATUS_OK);
    REQUIRE(memcmp(&before, &result, sizeof result) == 0);
    return 0;
}
