/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_readiness.c
 *
 * PURPOSE:
 *   Verify readiness admission, nominal budgets and preview contracts without model downloads or endpoint requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int Limits(const char *scenario)
{
    UmiModelReadinessPlan plan = {0};
    plan.port = 8080U; plan.attempts = 3U;
    plan.interval_millis = 50U; plan.timeout_millis = 100U;
    uint32_t expected = 400U;
    bool valid = false;
    if (!strcmp(scenario, "budget")) valid = true;
    else if (!strcmp(scenario, "one")) { plan.attempts = 1U; plan.timeout_millis = 60000U; expected = 60000U; valid = true; }
    else if (!strcmp(scenario, "boundary")) { plan.attempts = 2U; plan.interval_millis = 200U; plan.timeout_millis = 29900U; expected = 60000U; valid = true; }
    else if (!strcmp(scenario, "attempts-max")) { plan.attempts = 64U; expected = 9550U; valid = true; }
    else if (!strcmp(scenario, "port")) plan.port = 1023U;
    else if (!strcmp(scenario, "attempts-zero")) plan.attempts = 0U;
    else if (!strcmp(scenario, "attempts-over")) plan.attempts = 65U;
    else if (!strcmp(scenario, "pause-low")) plan.interval_millis = 49U;
    else if (!strcmp(scenario, "pause-high")) plan.interval_millis = 5001U;
    else if (!strcmp(scenario, "timeout-low")) plan.timeout_millis = 99U;
    else if (!strcmp(scenario, "timeout-high")) plan.timeout_millis = 60001U;
    else if (!strcmp(scenario, "budget-over")) { plan.attempts = 2U; plan.interval_millis = 200U; plan.timeout_millis = 29901U; }
    else if (!strcmp(scenario, "wide")) { plan.attempts = UINT32_MAX; plan.interval_millis = UINT32_MAX; plan.timeout_millis = UINT32_MAX; }
    else return 2;
    uint32_t budget = UINT32_MAX;
    UmiModelReadinessPlan output; unsigned char before[sizeof output];
    memset(&output, 0xa5, sizeof output); memcpy(before, &output, sizeof output);
    UmiStatus wanted = valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    CHECK(UmiModelReadinessBudget(&plan, &budget) == wanted);
    CHECK(UmiModelReadinessPlanCreate(plan.port, plan.attempts,
        plan.interval_millis, plan.timeout_millis, &output) == wanted);
    if (valid) {
        CHECK(budget == expected);
        CHECK(output.port == plan.port && output.attempts == plan.attempts);
        CHECK(output.interval_millis == plan.interval_millis && output.timeout_millis == plan.timeout_millis);
    } else {
        CHECK(budget == UINT32_MAX);
        CHECK(!memcmp(before, &output, sizeof output));
        UmiModelReadinessReport report; unsigned char saved[sizeof report];
        memset(&report, 0xa5, sizeof report); memcpy(saved, &report, sizeof report);
        CHECK(UmiModelReadinessWait(&plan, true, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(saved, &report, sizeof report));
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int limits = Limits(argv[1]);
    if (limits != 2) return limits;
    UmiModelReadinessPlan plan;
    CHECK(UmiModelReadinessPlanCreate(8080U, 3U, 50U, 100U, &plan) == UMI_STATUS_OK);
    UmiModelReadinessReport report; unsigned char before[sizeof report];
    memset(&report, 0xa5, sizeof report); memcpy(before, &report, sizeof report);
    if (!strcmp(argv[1], "null")) {
        uint32_t budget = UINT32_MAX;
        CHECK(UmiModelReadinessBudget(NULL, &budget) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(budget == UINT32_MAX);
        CHECK(UmiModelReadinessBudget(&plan, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelReadinessPlanCreate(8080U, 3U, 50U, 100U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelReadinessWait(NULL, true, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelReadinessWait(&plan, true, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(argv[1], "consent")) {
        CHECK(UmiModelReadinessWait(&plan, false, NULL, &report) == UMI_STATUS_PERMISSION_DENIED);
    } else if (!strcmp(argv[1], "cancelled")) {
        UmiAiWorkspaceCancellation *token = NULL;
        CHECK(UmiAiWorkspaceCancellationCreate(&token) == UMI_STATUS_OK);
        UmiAiWorkspaceCancellationRequest(token);
        UmiStatus status = UmiModelReadinessWait(&plan, true, token, &report);
        UmiAiWorkspaceCancellationDestroy(token);
        CHECK(status == UMI_STATUS_CANCELLED);
    } else if (!strcmp(argv[1], "preview")) {
        char *args[] = {"wait", "8080", "3", "50", "100"};
        CHECK(UmiModelsCommand(5, args) == 0);
    } else if (!strcmp(argv[1], "cli-rejection")) {
        char *bad[] = {"wait", "8080", "3", "50", "100", "--apply"};
        CHECK(UmiModelsCommand(6, bad) == 2);
        bad[5] = "--probe"; bad[2] = "0";
        CHECK(UmiModelsCommand(6, bad) == 2);
        bad[2] = "64"; bad[4] = "1000";
        CHECK(UmiModelsCommand(6, bad) == 2);
        bad[2] = "3"; bad[4] = "100"; bad[3] = "-1";
        CHECK(UmiModelsCommand(6, bad) == 2);
        bad[3] = "50"; bad[1] = "8080junk";
        CHECK(UmiModelsCommand(6, bad) == 2);
        CHECK(UmiModelsCommand(4, bad) == 2);
    } else if (!strcmp(argv[1], "unavailable")) {
        if (UmiAiWorkspaceLocalProviderAvailable()) {
            fputs("SKIP: this case requires a separate HTTP-OFF build.\n", stderr);
            return 77;
        }
        CHECK(UmiModelReadinessWait(&plan, true, NULL, &report) == UMI_STATUS_UNAVAILABLE);
        CHECK(report.attempts_completed == 1U && report.last_probe_status == UMI_STATUS_UNAVAILABLE);
        CHECK(!report.has_health && !report.last_health.endpoint_reports_ready && report.last_health.http_status == 0U);
        return 0;
    } else return 2;
    CHECK(!memcmp(before, &report, sizeof report));
    return 0;
}
