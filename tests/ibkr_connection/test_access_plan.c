/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_access_plan.c
 *
 * PURPOSE:
 *   Verify mode isolation, explicit Live consent and unchanged outputs on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_plan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #condition); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER, &plan) == UMI_STATUS_OK);
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    if (strcmp(argv[1], "guest") == 0) {
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_GUEST, &plan) == UMI_STATUS_OK);
        UmiIbkrConnectionOptions before;
        memcpy(&before, &options, sizeof before);
        CHECK(plan.port == 0U && plan.clientId == 0);
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, true, &options) == UMI_STATUS_UNAVAILABLE);
        CHECK(memcmp(&before, &options, sizeof options) == 0);
    } else if (strcmp(argv[1], "paper") == 0) {
        CHECK(plan.port == 7497U && plan.clientId == 35);
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, false, &options) == UMI_STATUS_OK);
        CHECK(options.environment == UMI_TRADING_PAPER && options.adapter.paperOnly == 1);
        CHECK(options.adapter.readOnly == 1 && strcmp(options.adapter.host, "127.0.0.1") == 0);
        CHECK(options.adapter.account[0] == 0);
    } else if (strcmp(argv[1], "live-consent") == 0) {
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
        UmiIbkrConnectionOptions before;
        memcpy(&before, &options, sizeof before);
        CHECK(plan.port == 7496U);
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, false, &options) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&before, &options, sizeof options) == 0);
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, true, &options) == UMI_STATUS_OK);
        CHECK(options.environment == UMI_TRADING_LIVE && options.adapter.paperOnly == 0);
        CHECK(options.adapter.readOnly == 1 && options.acknowledgeLive);
    } else if (strcmp(argv[1], "gateway") == 0) {
        CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
        CHECK(plan.port == 4002U);
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, false, &options) == UMI_STATUS_OK);
        CHECK(options.program == UMI_IBKR_GATEWAY && options.adapter.port == 4002U);
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
        CHECK(plan.port == 4001U);
    } else if (strcmp(argv[1], "invalid") == 0) {
        UmiBrokerAccessPlan before;
        memcpy(&before, &plan, sizeof before);
        CHECK(UmiBrokerAccessPlanCreate((UmiBrokerAccessMode)99, &plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &plan, sizeof plan) == 0);
        CHECK(UmiBrokerAccessPlanSetProgram(&plan, (UmiIbkrProgram)99) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &plan, sizeof plan) == 0);
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessPlanValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessPlanConnectionOptions(NULL, false, &options) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "endpoint") == 0) {
        plan.port = 5001U;
        plan.clientId = 42;
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, false, &options) == UMI_STATUS_OK);
        CHECK(options.adapter.port == 5001U && options.adapter.clientId == 42);
        UmiIbkrConnectionOptions before;
        memcpy(&before, &options, sizeof before);
        plan.clientId = 0;
        CHECK(UmiBrokerAccessPlanConnectionOptions(&plan, true, &options) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &options, sizeof options) == 0);
        plan.clientId = 35; plan.port = 0;
        CHECK(UmiBrokerAccessPlanValidate(&plan) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "guidance") == 0) {
        CHECK(strstr(UmiBrokerAccessAuthenticationGuide(UMI_BROKER_ACCESS_GUEST), "without credentials") != NULL);
        CHECK(strstr(UmiBrokerAccessAuthenticationGuide(UMI_BROKER_ACCESS_PAPER), "there") != NULL);
        CHECK(strcmp(UmiBrokerAccessModeName(UMI_BROKER_ACCESS_LIVE), "Live") == 0);
        CHECK(strcmp(UmiBrokerAccessModeName((UmiBrokerAccessMode)-1), "Unavailable") == 0);
    } else return 2;
    return 0;
}
