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

/* Provide the contains text operation used by this module and its client applications. */
static bool contains_text(const char *text, const char *part)
{
    return text != NULL && part != NULL && strstr(text, part) != NULL;
}

/* Provide the kind mask operation used by this module and its client applications. */
static uint64_t kind_mask(UmiContextKind kind)
{
    return umi_workbench_context_host_kind_mask(kind);
}

/*
 * Provide the is development capability operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the is finance capability operation used by this module and its client
 * applications.
 */
static bool is_finance_capability(const char *capability)
{
    return contains_text(capability, "banking") ||
           contains_text(capability, "payment") ||
           contains_text(capability, "accounting") ||
           contains_text(capability, "finance") ||
           contains_text(capability, "reconciliation") ||
           contains_text(capability, "digital-asset");
}

/*
 * Provide the is trading capability operation used by this module and its client
 * applications.
 */
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

/* Provide the is ai capability operation used by this module and its client applications. */
static bool is_ai_capability(const char *capability)
{
    return contains_text(capability, "ai") ||
           contains_text(capability, "llm") ||
           contains_text(capability, "knowledge") ||
           contains_text(capability, "retrieval");
}

/*
 * Provide the is media capability operation used by this module and its client
 * applications.
 */
static bool is_media_capability(const char *capability)
{
    return contains_text(capability, "media") ||
           contains_text(capability, "audio") ||
           contains_text(capability, "video") ||
           contains_text(capability, "game");
}

/*
 * Provide the is system capability operation used by this module and its client
 * applications.
 */
static bool is_system_capability(const char *capability)
{
    return contains_text(capability, "os") ||
           contains_text(capability, "process") ||
           contains_text(capability, "operations") ||
           contains_text(capability, "delivery") ||
           contains_text(capability, "metrics") ||
           contains_text(capability, "security");
}

/* Provide the resolve role operation used by this module and its client applications. */
static UmiWorkbenchContextHostPanelRole resolve_role(
    const UmiExperiencePanelDefinition *panel)
{
    const char *panel_id = panel->panel_id;
    const char *capability = panel->required_capability;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "context-inspector") ||
        strcmp(panel_id, "inspector") == 0) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(panel_id, "payments") == 0 ||
        contains_text(panel_id, "payment-approval")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "beneficiar") ||
        contains_text(panel_id, "customer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CUSTOMER;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "order-entry") ||
        contains_text(panel_id, "trade-capture")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "risk") ||
        contains_text(panel_id, "limits") ||
        contains_text(panel_id, "collateral")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_RISK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "trade") ||
        contains_text(panel_id, "position") ||
        contains_text(panel_id, "settlement")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "chart") ||
        contains_text(panel_id, "pricing") ||
        contains_text(panel_id, "analytics")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "source-control") ||
        contains_text(capability, "vcs")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SOURCE_CONTROL;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "test") ||
        contains_text(capability, "testing")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TEST_EXPLORER;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "debug") ||
        contains_text(capability, "debug")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DEBUGGER;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "terminal") ||
        contains_text(capability, "terminal")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TERMINAL;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(panel_id, "output") == 0 ||
        contains_text(panel_id, "logs")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_OUTPUT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "problem") ||
        contains_text(capability, "diagnostic")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_PROBLEMS;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(panel_id, "editor") == 0 ||
        strcmp(panel_id, "code") == 0 ||
        contains_text(capability, "editor")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "source-viewer") ||
        contains_text(panel_id, "document") ||
        contains_text(panel_id, "artifact")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DOCUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "canvas") ||
        contains_text(panel_id, "properties") ||
        contains_text(capability, "designer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_DESIGNER;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "timeline") ||
        contains_text(panel_id, "piano-roll") ||
        contains_text(panel_id, "mixer")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_MEDIA_TIMELINE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "model") ||
        contains_text(panel_id, "provider-router")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_MODEL;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "knowledge") ||
        contains_text(panel_id, "collection") ||
        contains_text(panel_id, "retrieval")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_KNOWLEDGE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "services") ||
        contains_text(panel_id, "engine-ops")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SERVICE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(panel_id, "system") ||
        contains_text(panel_id, "process") ||
        contains_text(panel_id, "storage") ||
        contains_text(panel_id, "network") ||
        contains_text(panel_id, "devices")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_SYSTEM;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (contains_text(capability, "treasury")) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_ai_capability(capability)) {
        return UMI_WORKBENCH_CONTEXT_HOST_PANEL_AI;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
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

/*
 * Initialise workbench context host experience panel policy from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_host_experience_panel_policy_init(
    UmiWorkbenchContextHostExperiencePanelPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Check that workbench context host experience panel policy satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_host_experience_panel_policy_validate(
    const UmiWorkbenchContextHostExperiencePanelPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || policy->structure_size != sizeof(*policy) ||
        policy->role < UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC ||
        policy->role > UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY ||
        policy->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        policy->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->include_endpoint && policy->accepted_kinds_mask == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW &&
        policy->published_kinds_mask != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host experience panel policy resolve operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_host_experience_panel_policy_resolve(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    UmiWorkbenchContextHostExperiencePanelPolicy *out_policy)
{
    const char *capability;
    uint64_t kinds;
    bool publishes;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || panel == NULL || out_policy == NULL ||
        panel->panel_id == NULL || panel->required_capability == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_context_host_experience_panel_policy_init(out_policy);
    capability = panel->required_capability;
    kinds = out_policy->accepted_kinds_mask;

    /* Apply this branch only when its contract condition is satisfied. */
    if (is_development_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_PROJECT) |
                 kind_mask(UMI_CONTEXT_KIND_WORKSPACE) |
                 kind_mask(UMI_CONTEXT_KIND_SOURCE_LOCATION);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_finance_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_ACCOUNT) |
                 kind_mask(UMI_CONTEXT_KIND_TRADE);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_trading_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_INSTRUMENT) |
                 kind_mask(UMI_CONTEXT_KIND_ACCOUNT) |
                 kind_mask(UMI_CONTEXT_KIND_TRADE);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_ai_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_PROJECT) |
                 kind_mask(UMI_CONTEXT_KIND_WORKSPACE);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (is_media_capability(capability)) {
        kinds |= kind_mask(UMI_CONTEXT_KIND_MEDIA) |
                 kind_mask(UMI_CONTEXT_KIND_PROJECT);
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the workbench context host experience group colour operation used by this module
 * and its client applications.
 */
UmiContextChannelColour umi_workbench_context_host_experience_group_colour(
    const char *context_group_id)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".red"))
        return UMI_CONTEXT_COLOUR_RED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".orange") ||
        contains_text(context_group_id, ".amber"))
        return UMI_CONTEXT_COLOUR_ORANGE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".yellow") ||
        contains_text(context_group_id, ".gold"))
        return UMI_CONTEXT_COLOUR_YELLOW;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".green"))
        return UMI_CONTEXT_COLOUR_GREEN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".cyan"))
        return UMI_CONTEXT_COLOUR_CYAN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".blue"))
        return UMI_CONTEXT_COLOUR_BLUE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".purple") ||
        contains_text(context_group_id, ".violet"))
        return UMI_CONTEXT_COLOUR_PURPLE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contains_text(context_group_id, ".magenta"))
        return UMI_CONTEXT_COLOUR_MAGENTA;
    return UMI_CONTEXT_COLOUR_NONE;
}

/*
 * Provide the workbench context host experience group title operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_experience_group_title(
    const char *context_group_id,
    char *out_title,
    size_t out_title_capacity)
{
    size_t index;
    size_t length = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context_group_id == NULL || context_group_id[0] == '\0' ||
        out_title == NULL || out_title_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (context_group_id[length] != '\0' &&
           context_group_id[length] != '.') {
        ++length;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > out_title_capacity) {
        out_title[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        char value = context_group_id[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '-' || value == '_') value = ' ';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == 0U && value >= 'a' && value <= 'z') {
            value = (char)(value - ('a' - 'A'));
        }
        out_title[index] = value;
    }
    out_title[length] = '\0';
    return UMI_STATUS_OK;
}
