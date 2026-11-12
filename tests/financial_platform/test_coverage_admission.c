/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_coverage_admission.c
 *
 * PURPOSE:
 *   Verify coverage boundaries and failure-preserving observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/financial_platform/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    UmiFinancialControlService *service = malloc(sizeof(*service));
    REQUIRE(service != NULL);
    umi_financial_control_service_init(service);
    UmiFinancialControlSnapshot direct, directBefore;
    UmiFinancialPlatformServiceSnapshot outer, outerBefore;
    memset(&direct, 0xa5, sizeof direct); memcpy(&directBefore, &direct, sizeof direct);
    memset(&outer, 0x5a, sizeof outer); memcpy(&outerBefore, &outer, sizeof outer);
    if (strcmp(test, "valid") == 0) {
        UmiStatus first = umi_financial_control_service_snapshot(service, &direct);
        UmiStatus second = umi_financial_platform_service_snapshot(service, &outer);
        free(service);
        REQUIRE(first == UMI_STATUS_OK && second == UMI_STATUS_OK);
        REQUIRE(direct.auditCount == 0 && !direct.auditReady && !outer.referenceDataReady && !outer.lifecycleReady);
        return 0;
    }
    if (strcmp(test, "rate-count") == 0) service->rates.count = SIZE_MAX;
    else if (strcmp(test, "relationship-count") == 0) service->relationships.count = SIZE_MAX;
    else if (strcmp(test, "instrument-count") == 0) service->instruments.count = SIZE_MAX;
    else if (strcmp(test, "trade-count") == 0) service->trades.count = SIZE_MAX;
    else if (strcmp(test, "position-count") == 0) service->positions.count = SIZE_MAX;
    else if (strcmp(test, "payment-count") == 0) service->payments.count = SIZE_MAX;
    else if (strcmp(test, "settlement-count") == 0) service->settlements.count = SIZE_MAX;
    else if (strcmp(test, "audit-count") == 0) service->audit.count = SIZE_MAX;
    else if (strcmp(test, "relationship_store-invalid") == 0) {
        UmiFinancialRelationship row = {"relationship", "party", "", "account", "", 1};
        REQUIRE(umi_financial_relationship_store_add(&service->relationships, &row) == UMI_STATUS_OK);
        memset(service->relationships.relationships[0].relationshipId, 'x', sizeof row.relationshipId);
    }
    else if (strcmp(test, "instrument_link_store-invalid") == 0) {
        UmiFinancialInstrumentLink row = {"instrument", "product", "GBP", "", 1U, 1};
        REQUIRE(umi_financial_instrument_link_store_upsert(&service->instruments, &row) == UMI_STATUS_OK);
        memset(service->instruments.links[0].instrumentId, 'x', sizeof row.instrumentId);
    }
    else if (strcmp(test, "trade_control_book-invalid") == 0) {
        UmiFinancialTradeControl row = {"trade", "account", "counterparty", "instrument", "GBP", 2, 100, 200, UMI_FINANCIAL_TRADE_DRAFT, 1U};
        REQUIRE(umi_financial_trade_control_book_add(&service->trades, &row) == UMI_STATUS_OK);
        memset(service->trades.trades[0].tradeId, 'x', sizeof row.tradeId);
    }
    else if (strcmp(test, "position_control_book-invalid") == 0) {
        UmiFinancialPositionControl row = {"account", "instrument", "GBP", 2, 90, 100, 5, 1U};
        REQUIRE(umi_financial_position_control_book_upsert(&service->positions, &row) == UMI_STATUS_OK);
        memset(service->positions.positions[0].accountId, 'x', sizeof row.accountId);
    }
    else if (strcmp(test, "payment_control_book-invalid") == 0) {
        UmiFinancialPaymentControl row = {"payment", "account", "creditor", "GBP", 100, UMI_FINANCIAL_PAYMENT_DRAFT, 1U, 0};
        REQUIRE(umi_financial_payment_control_book_add(&service->payments, &row) == UMI_STATUS_OK);
        memset(service->payments.payments[0].paymentId, 'x', sizeof row.paymentId);
    }
    else if (strcmp(test, "settlement_book-invalid") == 0) {
        UmiFinancialSettlementControl row = {"settlement", "trade", "account", "GBP", -100, 2, 1000U, UMI_FINANCIAL_SETTLEMENT_PENDING, 1U};
        REQUIRE(umi_financial_settlement_book_add(&service->settlements, &row) == UMI_STATUS_OK);
        memset(service->settlements.records[0].settlementId, 'x', sizeof row.settlementId);
    }
    else if (strcmp(test, "revision") == 0) service->relationships.revision = 0;
    else if (strcmp(test, "rate-invalid") == 0) {
        UmiFinancialRateRecord row = {"GBP", "USD", 1.25, 1U};
        REQUIRE(umi_financial_rate_book_upsert(&service->rates, &row) == UMI_STATUS_OK);
        service->rates.records[0].rate = NAN;
    } else if (strcmp(test, "audit-invalid") == 0) service->audit.gapCount = 1;
    else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_control_service_snapshot(NULL, &direct) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_platform_service_snapshot(NULL, &outer) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&direct, &directBefore, sizeof direct) == 0 && memcmp(&outer, &outerBefore, sizeof outer) == 0);
        free(service); return 0;
    } else { free(service); return 2; }
    UmiStatus first = umi_financial_control_service_snapshot(service, &direct);
    UmiStatus second = umi_financial_platform_service_snapshot(service, &outer);
    free(service);
    REQUIRE(first != UMI_STATUS_OK && second != UMI_STATUS_OK);
    REQUIRE(memcmp(&direct, &directBefore, sizeof direct) == 0 && memcmp(&outer, &outerBefore, sizeof outer) == 0);
    return 0;
}
