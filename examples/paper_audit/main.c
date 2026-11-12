/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/paper_audit/main.c
 *
 * PURPOSE:
 *   Exercise a local paper order lifecycle and inspect complete audit reasons through the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/paper_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "self-check") != 0 &&
        strcmp(argv[1], "duplicate") != 0 && strcmp(argv[1], "malformed") != 0)) {
        fputs("Umicom paper audit example: self-check|duplicate|malformed\n", stderr);
        return 2;
    }
    /* The runtime contains bounded journals. Allocate it once at a stable
     * address; a structure assignment would copy a self-pointer into the broker. */
    UmiPaperBrokerRuntime *runtime = malloc(sizeof(*runtime));
    if (runtime == NULL) return 1;
    UmiStatus status = umi_paper_broker_runtime_init(runtime);
    if (status == UMI_STATUS_OK) status = umi_paper_broker_runtime_connect(runtime, 1000U);
    UmiOrderRequest request = {0};
    strcpy(request.client_order_id.value, "practice-order");
    strcpy(request.account_id.value, "practice-account");
    strcpy(request.instrument.instrument_id.value, "practice-instrument");
    strcpy(request.instrument.symbol, "DEMO");
    strcpy(request.instrument.venue, "LOCAL");
    strcpy(request.instrument.currency.code, "USD");
    request.instrument.multiplier = 1.0;
    request.side = UMI_SIDE_BUY; request.type = UMI_ORDER_LIMIT; request.tif = UMI_TIF_DAY;
    request.quantity = 1.0; request.limit_price = 100.0; request.environment = UMI_TRADING_PAPER;
    if (status == UMI_STATUS_OK && strcmp(argv[1], "malformed") == 0) {
        UmiOrderRequest invalid = request; invalid.account_id.value[0] = '\0';
        size_t previous = runtime->audit.count;
        UmiStatus refusal = umi_paper_broker_runtime_submit(runtime, &invalid, 1050U);
        if (refusal != UMI_STATUS_INVALID_ARGUMENT || runtime->audit.count != previous ||
            runtime->orders.count != 0U) status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) status = umi_paper_broker_runtime_submit(runtime, &request, 1100U);
    if (status == UMI_STATUS_OK && strcmp(argv[1], "duplicate") == 0) {
        UmiStatus refusal = umi_paper_broker_runtime_submit(runtime, &request, 1150U);
        if (refusal != UMI_STATUS_ALREADY_EXISTS || runtime->orders.count != 1U)
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK)
        status = umi_paper_broker_runtime_cancel(runtime, &request.client_order_id, 1200U);
    if (status == UMI_STATUS_OK) {
        size_t expected = strcmp(argv[1], "duplicate") == 0 ? 4U : 3U;
        if (runtime->audit.count != expected ||
            runtime->storage.orders.orders[0].status != UMI_ORDER_CANCELLED ||
            runtime->orders.records[0].status != UMI_ORDER_CANCELLED) status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < runtime->audit.count; ++i) {
            const UmiBrokerAuditRecord *row = &runtime->audit.records[i];
            printf("%llu %s %s %s\n", (unsigned long long)row->sequence,
                   row->kind, row->entityId, umi_status_text(row->status));
        }
        puts("Local paper exercise complete. No network, files, cash movement or real orders.");
    } else fprintf(stderr, "Paper exercise: %s\n", umi_status_text(status));
    umi_paper_broker_runtime_destroy(runtime);
    free(runtime);
    return status == UMI_STATUS_OK ? 0 : 1;
}
