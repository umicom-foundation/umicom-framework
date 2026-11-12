/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_local_policy.c
 *
 * PURPOSE:
 *   Verify local time-in-force capability, consistent preview denials and admission before order identity consumption.
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
/* The real workspace owner permits capacity fault injection without a public
 * setter; all ordinary policy calls still use its public command entry points. */
#include "../../src/trading/workspace.c"
#define POLICY_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static UmiTimeInForce PolicyValue(const char *name)
{
    if (strstr(name, "-gtc")) return UMI_TIF_GTC;
    if (strstr(name, "-ioc") || !strcmp(name, "unsupported-final")) return UMI_TIF_IOC;
    if (strstr(name, "-fok")) return UMI_TIF_FOK;
    if (strstr(name, "-negative")) return (UmiTimeInForce)-1;
    if (strstr(name, "-unknown")) return (UmiTimeInForce)99;
    return UMI_TIF_DAY;
}
static int BrokerPolicy(const char *name, UmiTimeInForce tif)
{
    UmiSimulatedBrokerStorage *storage = calloc(1U, sizeof(*storage));
    UmiSimulatedBrokerStorage *before = malloc(sizeof(*before));
    POLICY_CHECK(storage != NULL && before != NULL);
    UmiBroker broker = {0};
    UmiOrderRequest request = test_order_request(); request.tif = tif;
    request.environment = strstr(name, "-paper-") ? UMI_TRADING_PAPER : UMI_TRADING_SIMULATION;
    POLICY_CHECK(umi_simulated_broker_create(storage, &broker) == UMI_STATUS_OK);
    POLICY_CHECK(broker.connect(broker.instance, request.environment) == UMI_STATUS_OK);
    memcpy(before, storage, sizeof(*storage));
    UmiStatus expected = tif == UMI_TIF_IOC || tif == UMI_TIF_FOK ? UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_OK;
    POLICY_CHECK(broker.submit_order(broker.instance, &request) == expected);
    if (expected == UMI_STATUS_OK) {
        POLICY_CHECK(storage->orders.count == 1U && storage->orders.orders[0].request.tif == tif);
        POLICY_CHECK(storage->orders.orders[0].status == UMI_ORDER_ACCEPTED);
    } else {
        POLICY_CHECK(!memcmp(before, storage, sizeof(*storage)));
    }
    broker.destroy(broker.instance); free(before); free(storage); return 0;
}
static int WorkspacePolicy(const char *name, UmiTimeInForce tif)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    UmiInstrument instrument = test_instrument(); instrument.multiplier = 1.0;
    POLICY_CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    POLICY_CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    POLICY_CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    POLICY_CHECK(umi_trading_workspace_set_draft_type(workspace, UMI_ORDER_LIMIT, tif) == UMI_STATUS_OK);
    POLICY_CHECK(umi_trading_workspace_set_draft_quantity(workspace, 2.0) == UMI_STATUS_OK);
    POLICY_CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    int preview = !strncmp(name, "preview-", 8U), timed = !strncmp(name, "timed-", 6U);
    int unsupported = tif == UMI_TIF_IOC || tif == UMI_TIF_FOK;
    int exhausted = strstr(name, "-exhausted") != NULL;
    if (exhausted) {
        if (!strcmp(name, "sequence-exhausted")) workspace->next_order_sequence = UINT64_MAX;
        else workspace->revision = UINT64_MAX;
    }
    if (!strcmp(name, "sequence-last")) workspace->next_order_sequence = UINT64_MAX - 1U;
    if (!strcmp(name, "unsupported-final")) workspace->revision = UINT64_MAX - 1U;
    UmiTradingWorkspaceSnapshot snapshot;
    POLICY_CHECK(umi_trading_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    POLICY_CHECK(snapshot.can_submit_order == !(unsupported || exhausted));
    UmiTradingWorkspace *before = malloc(sizeof(*before));
    POLICY_CHECK(before != NULL); memcpy(before, workspace, sizeof(*before));
    UmiRiskDecision decision, untouched;
    memset(&decision, 0xa5, sizeof(decision)); memcpy(&untouched, &decision, sizeof(untouched));
    UmiStatus expected = exhausted ? UMI_STATUS_CAPACITY_EXCEEDED :
        unsupported ? UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_OK;
    UmiStatus status = preview ? umi_trading_workspace_preview_order(workspace, &decision) :
        timed ? UmiTradingWorkspacePreviewOrderAt(workspace, 1000, &decision) :
        umi_trading_workspace_submit_order(workspace, 1000, &decision);
    POLICY_CHECK(status == expected);
    if (exhausted) {
        POLICY_CHECK(!memcmp(before, workspace, sizeof(*before)));
        POLICY_CHECK(!memcmp(&decision, &untouched, sizeof(decision)));
    } else if (unsupported) {
        POLICY_CHECK(!decision.allowed && workspace->has_draft_risk);
        POLICY_CHECK(workspace->revision == before->revision + 1U);
        POLICY_CHECK(workspace->next_order_sequence == before->next_order_sequence);
        POLICY_CHECK(!memcmp(&workspace->oms, &before->oms, sizeof(workspace->oms)));
        POLICY_CHECK(!memcmp(&workspace->draft_order, &before->draft_order, sizeof(workspace->draft_order)));
        POLICY_CHECK(workspace->executions.count == 0U && workspace->positions.count == 0U);
        UmiPretradeRiskEvidence evidence;
        POLICY_CHECK(UmiTradingWorkspaceRiskEvidence(workspace, &evidence) == UMI_STATUS_OK);
        POLICY_CHECK(!evidence.decision.allowed && !evidence.hasValuation);
        POLICY_CHECK(strstr(evidence.decision.reason, "immediate") != NULL);
    } else {
        POLICY_CHECK(decision.allowed && workspace->revision == before->revision + 1U);
        POLICY_CHECK(workspace->oms.orders.count == (preview || timed ? 0U : 1U));
        POLICY_CHECK(workspace->next_order_sequence == before->next_order_sequence + (preview || timed ? 0U : 1U));
    }
    free(before); umi_trading_workspace_destroy(workspace); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const char *known[] = {"value-day", "value-gtc", "value-ioc", "value-fok", "value-negative", "value-unknown", "broker-simulation-day", "broker-simulation-gtc", "broker-simulation-ioc", "broker-simulation-fok", "broker-paper-day", "broker-paper-gtc", "broker-paper-ioc", "broker-paper-fok", "preview-day", "preview-gtc", "preview-ioc", "preview-fok", "timed-day", "timed-gtc", "timed-ioc", "timed-fok", "submit-day", "submit-gtc", "submit-ioc", "submit-fok", "preview-exhausted", "timed-exhausted", "submit-exhausted", "sequence-exhausted", "sequence-last", "unsupported-final"};
    int recognized = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (!strcmp(name, known[i])) recognized = 1;
    if (!recognized) return 2;
    UmiTimeInForce tif = PolicyValue(name);
    if (!strncmp(name, "value-", 6U)) {
        UmiStatus expected = !umi_time_in_force_valid(tif) ? UMI_STATUS_INVALID_ARGUMENT :
            tif == UMI_TIF_IOC || tif == UMI_TIF_FOK ? UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_OK;
        POLICY_CHECK(UmiTimeInForceCheckLocalRetention(tif) == expected); return 0;
    }
    if (!strncmp(name, "broker-", 7U)) return BrokerPolicy(name, tif);
    return WorkspacePolicy(name, tif);
}
