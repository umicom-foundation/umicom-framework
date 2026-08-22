/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/trading_workbench.c
 *
 * PURPOSE:
 *   Compose reusable Trader and TMS groups/endpoints while leaving application-specific execution and market-data logic outside Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/trading_workbench.h"

#include <string.h>

#define UMI_TRADING_GROUP_ID "trading.red"
#define UMI_OPERATIONS_GROUP_ID "operations.green"

static UmiStatus add_group(
    UmiWorkbenchContextHostProfile *profile,
    const char *group_id,
    const char *title,
    UmiContextChannelColour colour,
    uint64_t kinds,
    bool active)
{
    UmiWorkbenchContextHostGroupDefinition group;
    UmiStatus status;

    umi_workbench_context_host_group_definition_init(
        &group, group_id);
    status = umi_workbench_context_host_copy_text(
        group.title, sizeof(group.title), title);
    if (status != UMI_STATUS_OK) return status;
    group.colour = colour;
    group.allowed_kinds_mask = kinds;
    group.default_mode =
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    group.default_active = active;
    return umi_workbench_context_host_profile_add_group(
        profile, &group);
}

static UmiStatus add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const char *endpoint_id,
    const char *panel_id,
    const char *application_id,
    const char *display_name,
    const char *group_id,
    UmiWorkbenchContextHostPanelRole role,
    UmiWorkbenchContextLinkMode mode,
    uint64_t accepted,
    uint64_t published)
{
    UmiWorkbenchContextHostEndpoint endpoint;
    UmiStatus status;

    umi_workbench_context_host_endpoint_init(
        &endpoint, endpoint_id);
    status = umi_workbench_context_host_endpoint_set_identity(
        &endpoint,
        panel_id,
        application_id,
        display_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_endpoint_set_group(
        &endpoint,
        group_id,
        mode);
    if (status != UMI_STATUS_OK) return status;

    endpoint.role = role;
    endpoint.state = UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE;
    endpoint.accepted_kinds_mask = accepted;
    endpoint.published_kinds_mask = published;
    endpoint.enabled = true;
    return umi_workbench_context_host_profile_add_endpoint(
        profile, &endpoint);
}

static UmiStatus build_trader(
    UmiWorkbenchContextHostProfile *profile)
{
    const uint64_t instrument =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_INSTRUMENT);
    const uint64_t account =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_ACCOUNT);
    const uint64_t trade =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_TRADE);
    const uint64_t selection =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_SELECTION);
    const uint64_t trading =
        instrument | account | trade | selection;
    const uint64_t operations =
        account | trade | selection;
    UmiStatus status;

    umi_workbench_context_host_profile_init(
        profile,
        "trader.linked-workbench",
        "org.umicom.trader");
    status = umi_workbench_context_host_profile_set_title(
        profile,
        "Umicom Trader Linked Workbench");
    if (status != UMI_STATUS_OK) return status;

    status = add_group(
        profile,
        UMI_TRADING_GROUP_ID,
        "Trading",
        UMI_CONTEXT_COLOUR_RED,
        trading,
        true);
    if (status != UMI_STATUS_OK) return status;

    status = add_group(
        profile,
        UMI_OPERATIONS_GROUP_ID,
        "Operations",
        UMI_CONTEXT_COLOUR_GREEN,
        operations,
        false);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.watchlist",
        "trader.watchlist",
        "org.umicom.trader",
        "Watchlist",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        instrument | selection,
        instrument | selection);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.chart",
        "trader.chart",
        "org.umicom.trader",
        "Chart",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        instrument | trade | selection,
        instrument | selection);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.order-entry",
        "trader.order-entry",
        "org.umicom.trader",
        "Order Entry",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW,
        instrument | account | trade,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.account-selector",
        "trader.account-selector",
        "org.umicom.trader",
        "Account Selector",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        account | selection,
        account | selection);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.risk",
        "trader.risk",
        "org.umicom.trader",
        "Risk",
        UMI_OPERATIONS_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_RISK,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        instrument | account | trade | selection,
        account | selection);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "trader.endpoint.trade-blotter",
        "trader.trade-blotter",
        "org.umicom.trader",
        "Trade Blotter",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        instrument | account | trade | selection,
        trade | selection);
    if (status != UMI_STATUS_OK) return status;

    return add_endpoint(
        profile,
        "trader.endpoint.context-inspector",
        "trader.context-inspector",
        "org.umicom.trader",
        "Context Inspector",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW,
        trading,
        0U);
}

static UmiStatus build_tms(
    UmiWorkbenchContextHostProfile *profile)
{
    const uint64_t instrument =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_INSTRUMENT);
    const uint64_t account =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_ACCOUNT);
    const uint64_t trade =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_TRADE);
    const uint64_t selection =
        umi_workbench_context_host_kind_mask(
            UMI_CONTEXT_KIND_SELECTION);
    const uint64_t trading =
        instrument | account | trade | selection;
    UmiStatus status;

    umi_workbench_context_host_profile_init(
        profile,
        "tms.linked-workbench",
        "org.umicom.tms");
    status = umi_workbench_context_host_profile_set_title(
        profile,
        "Umicom TMS Linked Workbench");
    if (status != UMI_STATUS_OK) return status;

    status = add_group(
        profile,
        UMI_TRADING_GROUP_ID,
        "Trading",
        UMI_CONTEXT_COLOUR_RED,
        trading,
        true);
    if (status != UMI_STATUS_OK) return status;

    status = add_group(
        profile,
        UMI_OPERATIONS_GROUP_ID,
        "Operations",
        UMI_CONTEXT_COLOUR_GREEN,
        account | trade | selection,
        false);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "tms.endpoint.trade-list",
        "tms.trade-list",
        "org.umicom.tms",
        "Trade List",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        trade | instrument | account | selection,
        trade | selection);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "tms.endpoint.trade-details",
        "tms.trade-details",
        "org.umicom.tms",
        "Trade Details",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW,
        trade | account | instrument,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        profile,
        "tms.endpoint.account",
        "tms.account",
        "org.umicom.tms",
        "Account",
        UMI_OPERATIONS_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        account | trade | selection,
        account | selection);
    if (status != UMI_STATUS_OK) return status;

    return add_endpoint(
        profile,
        "tms.endpoint.context-inspector",
        "tms.context-inspector",
        "org.umicom.tms",
        "Context Inspector",
        UMI_TRADING_GROUP_ID,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW,
        trading,
        0U);
}

UmiStatus umi_workbench_selection_provider_trading_workbench_build(
    UmiWorkbenchSelectionProviderTradingWorkbench *out_profile)
{
    UmiStatus status;
    if (out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_profile, 0, sizeof(*out_profile));

    status = umi_workbench_selection_provider_copy_text(
        out_profile->trading_group_id,
        sizeof(out_profile->trading_group_id),
        UMI_TRADING_GROUP_ID);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        out_profile->operations_group_id,
        sizeof(out_profile->operations_group_id),
        UMI_OPERATIONS_GROUP_ID);
    if (status != UMI_STATUS_OK) return status;

    status = build_trader(&out_profile->trader);
    if (status != UMI_STATUS_OK) return status;
    status = build_tms(&out_profile->tms);
    if (status != UMI_STATUS_OK) return status;

    out_profile->sources =
        umi_workbench_context_source_trading_profile_default();
    out_profile->sources.trading_group_id =
        out_profile->trading_group_id;
    out_profile->sources.operations_group_id =
        out_profile->operations_group_id;
    out_profile->revision = 1U;

    return umi_workbench_selection_provider_trading_workbench_validate(
        out_profile);
}

UmiStatus umi_workbench_selection_provider_trading_workbench_validate(
    const UmiWorkbenchSelectionProviderTradingWorkbench *profile)
{
    if (profile == NULL ||
        profile->trading_group_id[0] == '\0' ||
        profile->operations_group_id[0] == '\0' ||
        profile->sources.trader_application_id == NULL ||
        profile->sources.tms_application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_host_profile_validate(
            &profile->trader) != UMI_STATUS_OK ||
        umi_workbench_context_host_profile_validate(
            &profile->tms) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
