/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_support.c
 *
 * PURPOSE:
 *   Construct and destroy a two-group, three-endpoint context-host fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"

#include <string.h>

static uint64_t mask(UmiContextKind kind)
{
    return umi_workbench_context_host_kind_mask(kind);
}

static UmiStatus add_group(
    UmiWorkbenchContextHostProfile *profile,
    const char *id,
    const char *title,
    UmiContextChannelColour colour,
    uint64_t allowed,
    bool active)
{
    UmiWorkbenchContextHostGroupDefinition group;
    UmiStatus status;
    umi_workbench_context_host_group_definition_init(&group, id);
    status = umi_workbench_context_host_copy_text(
        group.title, sizeof(group.title), title);
    if (status != UMI_STATUS_OK) return status;
    group.colour = colour;
    group.allowed_kinds_mask = allowed;
    group.default_mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    group.default_active = active;
    return umi_workbench_context_host_profile_add_group(profile, &group);
}

static UmiStatus add_endpoint(
    UmiWorkbenchContextHostProfile *profile,
    const char *endpoint_id,
    const char *panel_id,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode,
    uint64_t accepted,
    uint64_t published)
{
    UmiWorkbenchContextHostEndpoint endpoint;
    UmiStatus status;
    umi_workbench_context_host_endpoint_init(&endpoint, endpoint_id);
    status = umi_workbench_context_host_endpoint_set_identity(
        &endpoint, panel_id, "org.umicom.test", panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_endpoint_set_group(
        &endpoint, group_id, mode);
    if (status != UMI_STATUS_OK) return status;
    endpoint.role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC;
    endpoint.accepted_kinds_mask = accepted;
    endpoint.published_kinds_mask = published;
    endpoint.state = UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE;
    return umi_workbench_context_host_profile_add_endpoint(profile, &endpoint);
}

UmiStatus umi_wch_test_fixture_init(UmiWorkbenchContextHostTestFixture *fixture)
{
    UmiWorkbenchContextHostConfig config;
    const uint64_t all = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    const uint64_t trading =
        mask(UMI_CONTEXT_KIND_INSTRUMENT) |
        mask(UMI_CONTEXT_KIND_ACCOUNT) |
        mask(UMI_CONTEXT_KIND_TRADE) |
        mask(UMI_CONTEXT_KIND_SELECTION);
    UmiStatus status;

    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(fixture, 0, sizeof(*fixture));

    umi_workbench_context_link_slave_controller_init(
        &fixture->link_controller);
    status = umi_workbench_context_link_slave_controller_start(
        &fixture->link_controller);
    if (status != UMI_STATUS_OK) return status;

    config = umi_workbench_context_host_config_default();
    config.host_id = "test.workbench";
    config.application_id = "org.umicom.test";
    config.observer_panel_id = "test.source";
    status = umi_workbench_context_host_create(
        &config,
        umi_workbench_context_link_slave_controller_service(
            &fixture->link_controller),
        &fixture->host);
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_context_host_profile_init(
        &fixture->profile,
        "test.profile",
        "org.umicom.test");
    status = umi_workbench_context_host_profile_set_title(
        &fixture->profile,
        "Test Profile");
    if (status != UMI_STATUS_OK) return status;

    status = add_group(
        &fixture->profile,
        "blue",
        "Blue",
        UMI_CONTEXT_COLOUR_BLUE,
        all,
        true);
    if (status != UMI_STATUS_OK) return status;
    status = add_group(
        &fixture->profile,
        "red",
        "Red",
        UMI_CONTEXT_COLOUR_RED,
        trading,
        false);
    if (status != UMI_STATUS_OK) return status;

    status = add_endpoint(
        &fixture->profile,
        "test.source.endpoint",
        "test.source",
        "blue",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        all,
        all);
    if (status != UMI_STATUS_OK) return status;
    status = add_endpoint(
        &fixture->profile,
        "test.target.endpoint",
        "test.target",
        "blue",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW,
        all,
        0U);
    if (status != UMI_STATUS_OK) return status;
    status = add_endpoint(
        &fixture->profile,
        "test.trading.endpoint",
        "test.trading",
        "red",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL,
        trading,
        trading);
    if (status != UMI_STATUS_OK) return status;

    return umi_workbench_context_host_apply_profile(
        fixture->host,
        &fixture->profile);
}

void umi_wch_test_fixture_destroy(UmiWorkbenchContextHostTestFixture *fixture)
{
    if (fixture == NULL) return;
    umi_workbench_context_host_destroy(fixture->host);
    fixture->host = NULL;
    (void)umi_workbench_context_link_slave_controller_stop(
        &fixture->link_controller);
    umi_workbench_context_link_slave_controller_destroy(
        &fixture->link_controller);
    memset(fixture, 0, sizeof(*fixture));
}

UmiStatus umi_wch_test_selection_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id)
{
    return umi_workbench_context_link_build_selection(
        out_payload,
        context_id,
        "org.umicom.test",
        source_panel_id,
        context_id,
        "test-selection",
        "item-1");
}

UmiStatus umi_wch_test_project_payload(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *source_panel_id)
{
    return umi_workbench_context_link_build_project(
        out_payload,
        context_id,
        "org.umicom.test",
        source_panel_id,
        "project-1",
        "C:/dev/project",
        "c");
}
