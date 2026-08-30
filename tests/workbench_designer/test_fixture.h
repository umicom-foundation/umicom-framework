/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_fixture.h
 *
 * PURPOSE:
 *   Provide deterministic semantic layout, service, controller and assertion
 *   fixtures shared by focused Layout Designer tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TEST_FIXTURE_H
#define UMICOM_WORKBENCH_DESIGNER_TEST_FIXTURE_H

#include <stdio.h>
#include <string.h>

#include "umicom/workbench_designer/workbench_designer.h"
#include "umicom/workbench_layout/memory_store.h"

#define TEST_REQUIRE(condition) \
    do { \
        if (!(condition)) { \
            (void)fprintf( \
                stderr, "%s:%d: requirement failed: %s\n", \
                __FILE__, __LINE__, #condition); \
            return 1; \
        } \
    } while (0)

#define TEST_REQUIRE_STATUS(expression) \
    do { \
        const UmiStatus test_status = (expression); \
        if (test_status != UMI_STATUS_OK) { \
            (void)fprintf( \
                stderr, "%s:%d: status %d from %s\n", \
                __FILE__, __LINE__, (int)test_status, #expression); \
            return 1; \
        } \
    } while (0)

#if defined(__GNUC__) || defined(__clang__)
#define TEST_UNUSED __attribute__((unused))
#else
#define TEST_UNUSED
#endif

static TEST_UNUSED UmiStatus test_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    const size_t length = source != NULL ? strlen(source) : 0U;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static TEST_UNUSED UmiStatus test_add_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *title,
    UmiWorkbenchLayoutNodeKind kind,
    const char *component_id,
    UmiWorkbenchLayoutRect bounds)
{
    UmiWorkbenchLayoutNode node;
    UmiStatus status;
    umi_workbench_layout_node_init(&node, node_id, kind);
    if (kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
        status = umi_workbench_layout_node_set_split(
            &node, UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL, 0.5);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_workbench_layout_node_set_title(&node, title);
    if (status != UMI_STATUS_OK) return status;
    if (component_id != NULL && component_id[0] != '\0') {
        status = umi_workbench_layout_node_set_component(
            &node, component_id, "org.umicom.studio");
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_workbench_layout_node_set_bounds(&node, &bounds);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_document_add_node(document, &node, NULL);
}

static TEST_UNUSED UmiStatus test_make_document(UmiWorkbenchLayoutDocument *document)
{
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutAudit audit;
    UmiWorkbenchLayoutRect root_bounds = {0, 0, 1280, 800};
    UmiWorkbenchLayoutRect left_bounds = {0, 0, 280, 800};
    UmiWorkbenchLayoutRect right_bounds = {280, 0, 1000, 800};
    UmiWorkbenchLayoutRect editor_bounds = {280, 0, 1000, 590};
    UmiWorkbenchLayoutRect bottom_bounds = {280, 590, 1000, 210};
    UmiWorkbenchLayoutRect output_bounds = {280, 590, 500, 210};
    UmiWorkbenchLayoutRect terminal_bounds = {780, 590, 500, 210};
    UmiWorkbenchLayoutNode *root;
    UmiWorkbenchLayoutNode *right;
    UmiStatus status;

    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_layout_document_init(
        document, "layout.fixture", "Fixture Layout");
    (void)memset(&identity, 0, sizeof(identity));
    (void)test_copy_text(
        identity.layout_id, sizeof(identity.layout_id), "layout.fixture");
    (void)test_copy_text(
        identity.owner_user_id, sizeof(identity.owner_user_id), "user.sammy");
    (void)test_copy_text(
        identity.owner_application_id,
        sizeof(identity.owner_application_id), "org.umicom.studio");
    (void)test_copy_text(
        identity.workspace_id, sizeof(identity.workspace_id), "workspace.fixture");
    status = umi_workbench_layout_document_set_identity(document, &identity);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&audit, 0, sizeof(audit));
    (void)test_copy_text(audit.created_by, sizeof(audit.created_by), "user.sammy");
    (void)test_copy_text(audit.modified_by, sizeof(audit.modified_by), "user.sammy");
    audit.created_at_ms = 1000U;
    audit.modified_at_ms = 1000U;
    status = umi_workbench_layout_document_set_audit(document, &audit);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_set_metadata(
        document, "Fixture Layout", "development",
        "Deterministic workbench layout used by focused designer tests.");
    if (status != UMI_STATUS_OK) return status;

    status = test_add_node(document, "root", "Root Split",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT, "", root_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "project", "Project Explorer",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL, "studio.project-explorer", left_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "right", "Work Area",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT, "", right_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "editor", "Editor",
        UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP, "studio.editor", editor_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "bottom", "Bottom Tabs",
        UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP, "", bottom_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "output", "Output",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL, "studio.output", output_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = test_add_node(document, "terminal", "Terminal",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL, "studio.terminal", terminal_bounds);
    if (status != UMI_STATUS_OK) return status;

    root = umi_workbench_layout_document_find_node_mutable(document, "root");
    right = umi_workbench_layout_document_find_node_mutable(document, "right");
    if (root == NULL || right == NULL) return UMI_STATUS_INTERNAL_ERROR;
    status = umi_workbench_layout_node_set_split(
        root, UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL, 0.22);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_node_set_split(
        right, UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL, 0.74);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "root", "project", 0U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "root", "right", 1U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "right", "editor", 0U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "right", "bottom", 1U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "bottom", "output", 0U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_attach_child(
        document, "bottom", "terminal", 1U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_set_root(document, "root");
    if (status != UMI_STATUS_OK) return status;
    document->nodes[
        umi_workbench_layout_document_find_node_index(document, "bottom")]
        .active_child_index = 0U;
    umi_workbench_layout_document_refresh_hash(document);
    return UMI_STATUS_OK;
}

static TEST_UNUSED UmiStatus test_make_service(
    UmiWorkbenchDesignerService **out_service,
    UmiWorkbenchDesignerSession **out_session)
{
    UmiWorkbenchDesignerServiceConfig config =
        umi_workbench_designer_service_config_default();
    UmiWorkbenchLayoutDocument document;
    UmiStatus status;
    if (out_service == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    *out_session = NULL;
    status = test_make_document(&document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_service_create(&config, out_service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_service_open(
        *out_service, "session.fixture", &document, NULL, out_session);
    if (status != UMI_STATUS_OK) {
        umi_workbench_designer_service_destroy(*out_service);
        *out_service = NULL;
        return status;
    }
    return UMI_STATUS_OK;
}

static TEST_UNUSED UmiStatus test_make_controller(
    UmiWorkbenchDesignerService **out_service,
    UmiWorkbenchDesignerController *out_controller,
    UmiWorkbenchDesignerSession **out_session)
{
    UmiStatus status = test_make_service(out_service, out_session);
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_designer_controller_init(
        out_controller, "layout-designer", *out_service);
    status = umi_workbench_designer_controller_initialise(out_controller);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_designer_controller_start(out_controller);
}

#endif
