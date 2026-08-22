/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/trading_profile.c
 *
 * PURPOSE:
 *   Register watchlist, chart, account, risk and TMS trade sources over the same generic event/context pipeline used by Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/trading_profile.h"

UmiWorkbenchContextSourceTradingProfile
umi_workbench_context_source_trading_profile_default(void)
{
    UmiWorkbenchContextSourceTradingProfile profile;
    profile.trader_application_id = "org.umicom.trader";
    profile.tms_application_id = "org.umicom.tms";
    profile.trading_group_id = "";
    profile.operations_group_id = "";
    return profile;
}

static UmiStatus register_source(
    UmiWorkbenchContextSourceService *service,
    const char *application_id,
    const char *source_id,
    const char *panel_id,
    const char *display_name,
    UmiWorkbenchContextSourceKind source_kind,
    UmiContextKind context_kind,
    const char *preferred_group,
    uint64_t window_ms,
    uint64_t minimum_interval_ms)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiStatus status;

    umi_workbench_context_source_definition_init(
        &definition, source_id);
    status = umi_workbench_context_source_definition_set_identity(
        &definition,
        application_id,
        panel_id,
        display_name);
    if (status != UMI_STATUS_OK) return status;
    if (preferred_group != NULL && preferred_group[0] != '\0') {
        status = umi_workbench_context_source_definition_set_group(
            &definition, preferred_group);
        if (status != UMI_STATUS_OK) return status;
    }
    definition.source_kind = source_kind;
    definition.trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT;
    definition.context_kind = context_kind;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)context_kind - 1U);
    definition.coalescing_window_ms = window_ms;
    definition.minimum_interval_ms = minimum_interval_ms;

    return umi_workbench_context_source_service_register(
        service, &definition);
}

UmiStatus umi_workbench_context_source_register_trading_profile(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceTradingProfile *profile)
{
    UmiWorkbenchContextSourceTradingProfile effective;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    effective = profile != NULL
        ? *profile
        : umi_workbench_context_source_trading_profile_default();
    if (effective.trader_application_id == NULL ||
        effective.tms_application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = register_source(
        service,
        effective.trader_application_id,
        "trader.watchlist.selection",
        "trader.watchlist",
        "Trader Watchlist selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST,
        UMI_CONTEXT_KIND_INSTRUMENT,
        effective.trading_group_id,
        25U,
        5U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        effective.trader_application_id,
        "trader.chart.selection",
        "trader.chart",
        "Trader Chart instrument",
        UMI_WORKBENCH_CONTEXT_SOURCE_CHART,
        UMI_CONTEXT_KIND_INSTRUMENT,
        effective.trading_group_id,
        25U,
        5U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        effective.trader_application_id,
        "trader.account.selection",
        "trader.account-selector",
        "Trader Account selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTOR,
        UMI_CONTEXT_KIND_ACCOUNT,
        effective.trading_group_id,
        25U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        effective.trader_application_id,
        "trader.risk.account-selection",
        "trader.risk",
        "Trader Risk account selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_RISK,
        UMI_CONTEXT_KIND_ACCOUNT,
        effective.operations_group_id,
        25U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    return register_source(
        service,
        effective.tms_application_id,
        "tms.trade.selection",
        "tms.trade-list",
        "TMS Trade selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_TRADE_BLOTTER,
        UMI_CONTEXT_KIND_TRADE,
        effective.trading_group_id,
        25U,
        0U);
}
