/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_reconciler_boundaries.c
 * PURPOSE: Verify fill identity and counter checks precede every reconciliation mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/execution_reconciler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Run(UmiBrokerExecutionReconciler *r, UmiBrokerExecutionReconciler *before, const char *mode)
{
    umi_broker_execution_reconciler_init(r);
    UmiExecutionReport report = {0};
    strcpy(report.execution_id.value, "execution-1");
    strcpy(report.client_order_id.value, "order-1");
    report.fill_quantity = 10.0;
    report.fill_price = 1.23;
    report.event_time_ms = 1000;
    CHECK(umi_broker_execution_reconciler_accept(r, &report) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(mode, "duplicates-capacity"))
    {
        r->duplicates = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "corrections-capacity"))
    {
        r->corrections = UINT64_MAX;
        report.fill_price = 1.24;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "revision"))
    {
        r->revision = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "late-capacity"))
    {
        r->lateReports = UINT64_MAX;
        strcpy(report.execution_id.value, "execution-2");
        report.event_time_ms = 900;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "other-order"))
    {
        strcpy(report.client_order_id.value, "order-2");
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "count"))
    {
        r->count = UMI_BROKER_EXECUTION_CAPACITY + 1U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "stored-id"))
    {
        memset(r->reports[0].execution_id.value, 'x', sizeof r->reports[0].execution_id.value);
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "duplicate-stored"))
    {
        r->reports[1] = r->reports[0];
        r->count = 2U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "high-water"))
    {
        report.event_time_ms = 1500;
        report.fill_price = 1.24;
    }
    else if (!strcmp(mode, "late"))
    {
        strcpy(report.execution_id.value, "execution-2");
        report.event_time_ms = 900;
    }
    else if (!strcmp(mode, "invalid-report"))
    {
        report.fill_quantity = 0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "duplicate"))
        return 2;
    *before = *r;
    CHECK(umi_broker_execution_reconciler_accept(r, &report) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(r, before, sizeof *r) == 0);
    else if (!strcmp(mode, "high-water"))
    {
        CHECK(r->latestEventTimeMilliseconds == 1500 && r->corrections == 1U && r->count == 1U);
        strcpy(report.execution_id.value, "execution-2");
        report.event_time_ms = 1400;
        CHECK(umi_broker_execution_reconciler_accept(r, &report) == UMI_STATUS_OK);
        CHECK(r->lateReports == 1U);
    }
    else if (!strcmp(mode, "late"))
        CHECK(r->lateReports == 1U && r->count == 2U && r->latestEventTimeMilliseconds == 1000);
    else
        CHECK(r->duplicates == 1U && r->count == 1U);
    if (!strcmp(mode, "count") || !strcmp(mode, "stored-id") || !strcmp(mode, "duplicate-stored"))
        CHECK(umi_broker_execution_reconciler_find(r, "execution-1") == NULL);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiBrokerExecutionReconciler *r = calloc(1, sizeof *r), *before = calloc(1, sizeof *before);
    if (r == NULL || before == NULL)
    {
        free(r);
        free(before);
        return 1;
    }
    int result = Run(r, before, argv[1]);
    free(r);
    free(before);
    return result;
}
