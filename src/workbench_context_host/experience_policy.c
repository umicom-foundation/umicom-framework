/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/experience_policy.c
 *
 * PURPOSE:
 *   Resolve application panels to stable context kinds, endpoint roles and
 *   link modes using shared capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/experience_policy.h"

#include <string.h>

static bool contains_text(const char *text, const char *part)
{
    return text != NULL && part != NULL && strstr(text, part) != NULL;
}

static uint64_t kind_mask(UmiContextKind kind)
{
    return umi_workbench_context_host_kind_mask(kind);
}

static bool is_development_capability(const char *capability)
{
    return contains_text(capability, "workspace") ||
           contains_text(capability, "project") ||
           contains_text(capability, "editor") ||
           contains_text(capability, "diagnostic") ||
           contains_text(capability, "vcs") ||
           contains_text(capability, "testing") ||
           contains_text(capability, "debug") ||
           contains_text(capability, "designer") ||
           contains_text(capability, "web") ||
           contains_text(capability, "database") ||
           contains_text(capability, "language");
}

static bool is_finance_capability(const char *capability)
{
    return contains_text(capability, "banking") ||
           contains_text(capability, "payment") ||
           contains_text(capability, "accounting") ||
           contains_text(capability, "finance") ||
           contains_text(capability, "reconciliation") ||
           contains_text(capability, "digital-asset");
}

static bool is_trading_capability(const char *capability)
{
    return contains_text(capability, "trading") ||
           contains_text(capability, "market-data") ||
           contains_text(capability, "position") ||
           contains_text(capability, "valuation") ||
           contains_text(capability, "risk") ||
           contains_text(capability, "collateral") ||
           contains_text(capability, "settlement") ||
           contains_text(capability, "tms");
}

static bool is_ai_capability(const char *capability)
{
    return contains_text(capability, "ai") ||
           contains_text(capability, "llm") ||
           contains_text(capability, "knowledge") ||
           contains_text(capability, "retrieval");
}

static bool is_media_capability(const char *capability)
{
    return contains_text(capability, "media") ||
           contains_text(capability, "audio") ||
           contains_text(capability, "video") ||
           contains_text(capability, "game");
}

static bool is_system_capability(const char *capability)
{
    return contains_text(capability, "os") ||
           contains_text(capability, "process") ||
           contains_text(capability, "operations") ||
           contains_text(capability, "delivery") ||
           contains_text(capability, "metrics") ||
           contains_text(capability, "security");
}

static UmiWorkbenchContextHostPanelRole resolve_role(
    const UmiExperiencePanelDefinition *panel)
{
    const char *panel_id = panel->panel_id;
    const char *capability = panel->required_capability;

    if (contains_text(panel_id, "context-inspector") ||
        strcmp(panel_id, "inspector") == 0) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR;
    }
    if (strcmp(panel_id, "payments") == 0 ||
        contains_text(panel_id, "payment-approval")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT;
    }
    if (contains_text(panel_id, "beneficiar") ||
        contains_text(panel_id, "customer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CUSTOMER;
    }
    if (contains_text(panel_id, "order-entry") ||
        contains_text(panel_id, "trade-capture")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY;
    }
    if (contains_text(panel_id, "risk") ||
        contains_text(panel_id, "limits") ||
        contains_text(panel_id, "collateral")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_RISK;
    }
    if (contains_text(panel_id, "trade") ||
        contains_text(panel_id, "position") ||
        contains_text(panel_id, "settlement")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE;
    }
    if (contains_text(panel_id, "chart") ||
        contains_text(panel_id, "pricing") ||
        contains_text(panel_id, "analytics")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART;
    }
    if (contains_text(panel_id, "source-control") ||
        contains_text(capability, "vcs")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SOURCE_CONTROL;
    }
    if (contains_text(panel_id, "test") ||
        contains_text(capability, "testing")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TEST_EXPLORER;
    }
    if (contains_text(panel_id, "debug") ||
        contains_text(capability, "debug")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DEBUGGER;
    }
    if (contains_text(panel_id, "terminal") ||
        contains_text(capability, "terminal")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TERMINAL;
    }
    if (strcmp(panel_id, "output") == 0 ||
        contains_text(panel_id, "logs")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_OUTPUT;
    }
    if (contains_text(panel_id, "problem") ||
        contains_text(capability, "diagnostic")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_PROBLEMS;
    }
    if (strcmp(panel_id, "editor") == 0 ||
        strcmp(panel_id, "code") == 0 ||
        contains_text(capability, "editor")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR;
    }
    if (contains_text(panel_id, "source-viewer") ||
        contains_text(panel_id, "document") ||
        contains_text(panel_id, "artifact")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DOCUMENT;
    }
    if (contains_text(panel_id, "canvas") ||
        contains_text(panel_id, "properties") ||
        contains_text(capability, "designer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DESIGNER;
    }
    if (contains_text(panel_id, "timeline") ||
        contains_text(panel_id, "piano-roll") ||
        contains_text(panel_id, "mixer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_MEDIA_TIMELINE;
    }
    if (contains_text(panel_id, "model") ||
        contains_text(panel_id, "provider-router")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_MODEL;
    }
    if (contains_text(panel_id, "knowledge") ||
        contains_text(panel_id, "collection") ||
        contains_text(panel_id, "retrieval")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_KNOWLEDGE;
    }
    if (contains_text(panel_id, "services") ||
        contains_text(panel_id, "engine-ops")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SERVICE;
    }
    if (contains_text(panel_id, "system") ||
        contains_text(panel_id, "process") ||
        contains_text(panel_id, "storage") ||
        contains_text(panel_id, "network") ||
        contains_text(panel_id, "devices")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SYSTEM;
    }
    if (contains_text(capability, "treasury")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY;
    }
    if (is_ai_capability(capability)) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_AI;
    }
    if (contains_text(panel_id, "explorer") ||
        contains_text(panel_id, "browser") ||
        contains_text(panel_id, "projects") ||
        contains_text(panel_id, "accounts") ||
        contains_text(panel_id, "collections") ||
        contains_text(panel_id, "devices")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER;
    }
    return UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC;
}

void umi_workbench_context_host_experience_panel_policy_init(
    UmiWorkbenchContextHostExperiencePanelPolicy *policy)
{
    if (policy == NULL) return;
    memset(policy, 0, sizeof(*policy));
    policy->structure_size = (uint32_t)sizeof(*policy);
    policy->role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC;
    policy->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW;
    policy->accepted_kinds_mask =
        kind_mask(UMI_CONTEXT_KIND_GENERIC) |
        kind_mask(UMI_CONTEXT_KIND_SELECTION);
    policy->include_endpoint = true;
}

UmiStatus umi_workbench_context_host_experience_panel_policy_validate(
    const UmiWorkbenchContextHostExperiencePanelPolicy *policy)
{
    if (policy == NULL || policy->structure_size != sizeof(*policy) ||
        policy->role < UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC ||
        policy->role > UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY ||
        policy->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        policy->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (policy->include_endpoint && policy->accepted_kinds_mask == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (policy->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW &&
        policy->published_kinds_mask != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_experience_panel_policy_resolve(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    UmiWorkbenchContextHostExperiencePanelPolicy *out_policy)
{
    const char *capability;
    uint64_t kinds;
    bool publishes;

    if (experience == NULL || panel == NULL || out_policy == NULL ||
        panel->panel_id == NULL || panel->required_capability == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_context_host_experience_panel_policy_init(out_policy);
    capability = panel->required_capability;
    kinds = out_policy->accepted_kinds_mask;

    if (is_development_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_PROJECT) |
                 kind_mask(UMI_CONTEXT_KIND_WORKSPACE) |
                 kind_mask(UMI_CONTEXT_KIND_SOURCE_LOCATION);
    }
    if (is_finance_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_ACCOUNT) |
                 kind_mask(UMI_CONTEXT_KIND_TRADE);
    }
    if (is_trading_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_INSTRUMENT) |
                 kind_mask(UMI_CONTEXT_KIND_ACCOUNT) |
                 kind_mask(UMI_CONTEXT_KIND_TRADE);
    }
    if (is_ai_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_PROJECT) |
                 kind_mask(UMI_CONTEXT_KIND_WORKSPACE);
    }
    if (is_media_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_MEDIA) |
                 kind_mask(UMI_CONTEXT_KIND_PROJECT);
    }
    if (is_system_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_WORKSPACE);
    }

    out_policy->role = resolve_role(panel);
    out_policy->accepted_kinds_mask = kinds;
    out_policy->include_endpoint = panel->context_group_id != NULL &&
                                   panel->context_group_id[0] != '\0';

    publishes = (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) != 0U &&
                (panel->flags & UMI_EXPERIENCE_PANEL_READ_ONLY) == 0U &&
                out_policy->role !=
                    UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR &&
                out_policy->role !=
                    UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY &&
                out_policy->role !=
                    UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT;
    out_policy->mode = publishes
        ? UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL
        : UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW;
    out_policy->published_kinds_mask = publishes ? kinds : 0U;

    return umi_workbench_context_host_experience_panel_policy_validate(
        out_policy);
}

UmiContextChannelColour umi_workbench_context_host_experience_group_colour(
    const char *context_group_id)
{
    if (contains_text(context_group_id, ".red"))
        return UMI_CONTEXT_COLOUR_RED;
    if (contains_text(context_group_id, ".orange") ||
        contains_text(context_group_id, ".amber"))
        return UMI_CONTEXT_COLOUR_ORANGE;
    if (contains_text(context_group_id, ".yellow") ||
        contains_text(context_group_id, ".gold"))
        return UMI_CONTEXT_COLOUR_YELLOW;
    if (contains_text(context_group_id, ".green"))
        return UMI_CONTEXT_COLOUR_GREEN;
    if (contains_text(context_group_id, ".cyan"))
        return UMI_CONTEXT_COLOUR_CYAN;
    if (contains_text(context_group_id, ".blue"))
        return UMI_CONTEXT_COLOUR_BLUE;
    if (contains_text(context_group_id, ".purple") ||
        contains_text(context_group_id, ".violet"))
        return UMI_CONTEXT_COLOUR_PURPLE;
    if (contains_text(context_group_id, ".magenta"))
        return UMI_CONTEXT_COLOUR_MAGENTA;
    return UMI_CONTEXT_COLOUR_NONE;
}

UmiStatus umi_workbench_context_host_experience_group_title(
    const char *context_group_id,
    char *out_title,
    size_t out_title_capacity)
{
    size_t index;
    size_t length = 0U;

    if (context_group_id == NULL || context_group_id[0] == '\0' ||
        out_title == NULL || out_title_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (context_group_id[length] != '\0' &&
           context_group_id[length] != '.') {
        ++length;
    }
    if (length + 1U > out_title_capacity) {
        out_title[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < length; ++index) {
        char value = context_group_id[index];
        if (value == '-' || value == '_') value = ' ';
        if (index == 0U && value >= 'a' && value <= 'z') {
            value = (char)(value - ('a' - 'A'));
        }
        out_title[index] = value;
    }
    out_title[length] = '\0';
    return UMI_STATUS_OK;
}
