/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_execution_admission.c
 *
 * PURPOSE:
 *   Verify bounded execution publication, final revisions and harmless retained replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* Fault injection uses the real owner layout, not a copied private struct or
 * a production setter. The archive supplies its remaining dependencies. */
#include "../../src/trading/workspace.c"
#define ADMISSION_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    UmiInstrument instrument = test_instrument(); instrument.multiplier = 1.0;
    UmiRiskDecision decision;
    ADMISSION_CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    ADMISSION_CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    ADMISSION_CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    ADMISSION_CHECK(umi_trading_workspace_set_draft_quantity(workspace, 4.0) == UMI_STATUS_OK);
    ADMISSION_CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    ADMISSION_CHECK(umi_trading_workspace_submit_order(workspace, 1000, &decision) == UMI_STATUS_OK);
    UmiOrder *order = &workspace->oms.orders.orders[0];
    UmiExecutionReport report = {0};
    strcpy(report.execution_id.value, "execution-admission");
    report.client_order_id = order->request.client_order_id;
    report.fill_quantity = !strcmp(name, "replay-filled") ? 4.0 : 1.0;
    report.fill_price = 100.0; report.event_time_ms = 1000;
    int existing = !strcmp(name, "existing-exhausted") || !strcmp(name, "last-existing");
    int replay = !strncmp(name, "replay-", 7U) || !strcmp(name, "conflict-exhausted");
    if (existing || replay || !strcmp(name, "execution-text")) {
        ADMISSION_CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
        if (existing || !strcmp(name, "execution-text"))
            strcpy(report.execution_id.value, "execution-next");
    }
    UmiStatus expected = UMI_STATUS_OK;
    int changes = 0;
    if (!strcmp(name, "new-exhausted") || !strcmp(name, "existing-exhausted")) {
        workspace->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name, "last-new") || !strcmp(name, "last-existing")) {
        workspace->revision = UINT64_MAX - 1U; changes = 1;
    } else if (!strcmp(name, "replay-exhausted") || !strcmp(name, "replay-filled")) {
        workspace->revision = UINT64_MAX;
    } else if (!strcmp(name, "conflict-exhausted")) {
        workspace->revision = UINT64_MAX; report.fill_price = 101.0;
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (!strcmp(name, "replay-cancelled")) {
        ADMISSION_CHECK(umi_trading_workspace_cancel_selected_order(workspace) == UMI_STATUS_OK);
        workspace->revision = UINT64_MAX;
    } else if (!strcmp(name, "order-count")) {
        workspace->oms.orders.count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "order-text")) {
        memset(order->request.client_order_id.value, 'x', sizeof(order->request.client_order_id.value));
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "execution-count")) {
        workspace->executions.count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "execution-text")) {
        memset(workspace->executions.reports[0].execution_id.value, 'x',
            sizeof(workspace->executions.reports[0].execution_id.value));
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "execution-full")) {
        for (size_t i = 0U; i < UMI_TRADING_MAX_ORDERS; ++i) {
            workspace->executions.reports[i] = report;
            (void)snprintf(workspace->executions.reports[i].execution_id.value,
                sizeof(report.execution_id.value), "retained-%zu", i);
        }
        workspace->executions.count = UMI_TRADING_MAX_ORDERS;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name, "position-count")) {
        workspace->positions.count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "position-full")) {
        for (size_t i = 0U; i < UMI_TRADING_MAX_POSITIONS; ++i) {
            workspace->positions.positions[i].instrument = instrument;
            (void)snprintf(workspace->positions.positions[i].instrument.instrument_id.value,
                sizeof(instrument.instrument_id.value), "other-%zu", i);
        }
        workspace->positions.count = UMI_TRADING_MAX_POSITIONS;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name, "order-revision")) {
        order->version = UINT64_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "missing-order")) {
        strcpy(report.client_order_id.value, "unretained"); expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(name, "invalid-report")) {
        report.fill_quantity = 0.0; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else { umi_trading_workspace_destroy(workspace); return 2; }

    /* The owner has large bounded stores. A heap snapshot observes unchanged
     * value fields on refusal without exhausting a small thread stack. */
    void *saved = malloc(sizeof(*workspace));
    ADMISSION_CHECK(saved != NULL); memcpy(saved, workspace, sizeof(*workspace));
    const uint64_t orderRevision = order->version;
    const size_t retained = workspace->executions.count;
    const double previousFill = order->filled_quantity;
    ADMISSION_CHECK(umi_trading_workspace_record_execution(workspace, &report) == expected);
    if (!changes) {
        ADMISSION_CHECK(!memcmp(saved, workspace, sizeof(*workspace)));
    } else {
        ADMISSION_CHECK(workspace->revision == UINT64_MAX);
        ADMISSION_CHECK(order->version == orderRevision + 1U);
        ADMISSION_CHECK(order->filled_quantity == previousFill + 1.0);
        ADMISSION_CHECK(workspace->executions.count == retained + 1U);
        ADMISSION_CHECK(workspace->positions.count == 1U);
        ADMISSION_CHECK(workspace->positions.positions[0].quantity == previousFill + 1.0);
        memcpy(saved, workspace, sizeof(*workspace));
        ADMISSION_CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
        ADMISSION_CHECK(!memcmp(saved, workspace, sizeof(*workspace)));
        strcpy(report.execution_id.value, "after-final-revision");
        ADMISSION_CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
        ADMISSION_CHECK(!memcmp(saved, workspace, sizeof(*workspace)));
    }
    free(saved);
    /* Restore intentionally damaged counts before normal owner teardown. */
    if (!strcmp(name, "order-count")) workspace->oms.orders.count = 1U;
    if (!strcmp(name, "execution-count")) workspace->executions.count = 0U;
    if (!strcmp(name, "position-count")) workspace->positions.count = 0U;
    umi_trading_workspace_destroy(workspace);
    return 0;
}
