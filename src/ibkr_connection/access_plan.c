/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/access_plan.c
 *
 * PURPOSE:
 *   Keep entry-mode defaults and broker authentication guidance in one shared owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_plan.h"

static bool ValidMode(UmiBrokerAccessMode mode)
{
    return mode == UMI_BROKER_ACCESS_GUEST ||
        mode == UMI_BROKER_ACCESS_PAPER || mode == UMI_BROKER_ACCESS_LIVE;
}

UmiStatus UmiBrokerAccessPlanValidate(const UmiBrokerAccessPlan *plan)
{
    if (plan == NULL || !ValidMode(plan->mode) ||
        (plan->program != UMI_IBKR_TWS && plan->program != UMI_IBKR_GATEWAY))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->mode == UMI_BROKER_ACCESS_GUEST)
        return plan->port == 0U && plan->clientId == 0
            ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    return plan->port != 0U && plan->clientId > 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus UmiBrokerAccessPlanCreate(UmiBrokerAccessMode mode,
    UmiBrokerAccessPlan *outPlan)
{
    if (outPlan == NULL || !ValidMode(mode)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrokerAccessPlan value = {0};
    value.mode = mode;
    value.program = UMI_IBKR_TWS;
    if (mode != UMI_BROKER_ACCESS_GUEST) {
        value.port = UmiIbkrDefaultPort(value.program,
            mode == UMI_BROKER_ACCESS_PAPER ? UMI_TRADING_PAPER : UMI_TRADING_LIVE);
        value.clientId = 35;
    }
    *outPlan = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiBrokerAccessPlanSetProgram(UmiBrokerAccessPlan *plan,
    UmiIbkrProgram program)
{
    if (UmiBrokerAccessPlanValidate(plan) != UMI_STATUS_OK ||
        (program != UMI_IBKR_TWS && program != UMI_IBKR_GATEWAY))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrokerAccessPlan value = *plan;
    value.program = program;
    if (value.mode != UMI_BROKER_ACCESS_GUEST)
        value.port = UmiIbkrDefaultPort(program,
            value.mode == UMI_BROKER_ACCESS_PAPER ? UMI_TRADING_PAPER : UMI_TRADING_LIVE);
    *plan = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiBrokerAccessPlanConnectionOptions(const UmiBrokerAccessPlan *plan,
    bool acknowledgeLive, UmiIbkrConnectionOptions *outOptions)
{
    if (outOptions == NULL || UmiBrokerAccessPlanValidate(plan) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->mode == UMI_BROKER_ACCESS_GUEST) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrConnectionOptions value = UmiIbkrConnectionOptionsDefault();
    value.program = plan->program;
    value.environment = plan->mode == UMI_BROKER_ACCESS_PAPER
        ? UMI_TRADING_PAPER : UMI_TRADING_LIVE;
    value.adapter.port = plan->port;
    value.adapter.clientId = plan->clientId;
    value.adapter.paperOnly = plan->mode == UMI_BROKER_ACCESS_PAPER ? 1 : 0;
    value.acknowledgeLive = acknowledgeLive;
    UmiStatus status = UmiIbkrConnectionValidate(&value);
    if (status != UMI_STATUS_OK) return status;
    *outOptions = value;
    return UMI_STATUS_OK;
}

const char *UmiBrokerAccessModeName(UmiBrokerAccessMode mode)
{
    switch (mode) {
    case UMI_BROKER_ACCESS_GUEST: return "Guest";
    case UMI_BROKER_ACCESS_PAPER: return "Paper";
    case UMI_BROKER_ACCESS_LIVE: return "Live";
    default: return "Unavailable";
    }
}

const char *UmiBrokerAccessAuthenticationGuide(UmiBrokerAccessMode mode)
{
    switch (mode) {
    case UMI_BROKER_ACCESS_GUEST:
        return "Open local simulation without credentials. Sample prices are not live market data.";
    case UMI_BROKER_ACCESS_PAPER:
        return "Select Paper in TWS or IB Gateway. Enter your paper username, password and any authentication code there. Then connect locally in Umicom.";
    case UMI_BROKER_ACCESS_LIVE:
        return "Select Live in TWS or IB Gateway. Enter your live username, password and authentication code there. Then explicitly confirm the Live connection in Umicom.";
    default: return "Choose Guest, Paper or Live before continuing.";
    }
}
