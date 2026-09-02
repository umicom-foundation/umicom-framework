/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_fixture.h
 *
 * PURPOSE:
 *   Provide deterministic semantic layout fixtures shared by focused
 *   workbench-layout tests without depending on a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_TEST_FIXTURE_H
#define UMICOM_WORKBENCH_LAYOUT_TEST_FIXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/workbench_layout/workbench_layout.h"

#define TEST_REQUIRE(condition, message)                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                     \
                          __FILE__, __LINE__, (message));                     \
            return 1;                                                         \
        }                                                                     \
    } while (0)

#define TEST_STATUS_OK(expression)                                            \
    do {                                                                      \
        UmiStatus test_status_value = (expression);                           \
        if (test_status_value != UMI_STATUS_OK) {                             \
            (void)fprintf(stderr,                                             \
                          "[FAIL] %s:%d: status=%d for %s\n",                \
                          __FILE__, __LINE__,                                 \
                          (int)test_status_value, #expression);               \
            return 1;                                                         \
        }                                                                     \
    } while (0)

/**
 * Exercise test copy text and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline void test_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    const size_t length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        (void)fprintf(stderr, "Fixture text exceeded its destination.\n");
        abort();
    }
    (void)memcpy(destination, source, length + 1U);
}

/**
 * Exercise test layout identity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiWorkbenchLayoutIdentity test_layout_identity(
    const char *layout_id)
{
    UmiWorkbenchLayoutIdentity identity;
    (void)memset(&identity, 0, sizeof(identity));
    test_copy_text(identity.layout_id, sizeof(identity.layout_id), layout_id);
    test_copy_text(
        identity.owner_user_id,
        sizeof(identity.owner_user_id),
        "developer");
    test_copy_text(
        identity.owner_application_id,
        sizeof(identity.owner_application_id),
        "org.umicom.studio");
    test_copy_text(
        identity.workspace_id,
        sizeof(identity.workspace_id),
        "workspace.framework");
    return identity;
}

/**
 * Exercise test layout principal and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiWorkbenchLayoutPrincipal test_layout_principal(void)
{
    UmiWorkbenchLayoutPrincipal principal;
    (void)memset(&principal, 0, sizeof(principal));
    principal.structure_size = sizeof(principal);
    test_copy_text(
        principal.user_id,
        sizeof(principal.user_id),
        "developer");
    test_copy_text(
        principal.workspace_id,
        sizeof(principal.workspace_id),
        "workspace.framework");
    principal.role = UMI_WORKBENCH_LAYOUT_ROLE_OWNER;
    principal.trusted_workspace = true;
    principal.administrator = false;
    return principal;
}

/**
 * Exercise test initialise node and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline void test_initialise_node(
    UmiWorkbenchLayoutNode *node,
    const char *node_id,
    const char *title,
    UmiWorkbenchLayoutNodeKind kind)
{
    umi_workbench_layout_node_init(node, node_id, kind);
    (void)umi_workbench_layout_node_set_title(node, title);
}

/**
 * Exercise test create development layout and return a clear result when the behaviour no
 * longer matches its contract.
 */
static inline UmiStatus test_create_development_layout(
    const char *layout_id,
    UmiWorkbenchLayoutDocument *document)
{
    UmiWorkbenchLayoutIdentity identity = test_layout_identity(layout_id);
    UmiWorkbenchLayoutNode root;
    UmiWorkbenchLayoutNode left;
    UmiWorkbenchLayoutNode centre;
    UmiWorkbenchLayoutNode right;
    UmiWorkbenchLayoutNode project;
    UmiWorkbenchLayoutNode editor;
    UmiWorkbenchLayoutNode chat;
    UmiWorkbenchLayoutNode output;
    size_t root_index;
    size_t left_index;
    size_t centre_index;
    size_t right_index;
    size_t project_index;
    size_t editor_index;
    size_t chat_index;
    size_t output_index;
    UmiStatus status;

    umi_workbench_layout_document_init(
        document, layout_id, "Development Workbench");
    status = umi_workbench_layout_document_set_identity(document, &identity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_document_set_metadata(
        document,
        "Development Workbench",
        "development",
        "Project, editor, chat and output panels in a portable semantic tree.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_workbench_layout_document_add_tag(document, "development");
    (void)umi_workbench_layout_document_add_tag(document, "studio");

    test_initialise_node(
        &root, "root", "Root", UMI_WORKBENCH_LAYOUT_NODE_SPLIT);
    (void)umi_workbench_layout_node_set_split(
        &root, UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL, 0.22);
    test_initialise_node(
        &left, "left", "Left Region", UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP);
    test_initialise_node(
        &centre,
        "centre",
        "Centre Region",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT);
    (void)umi_workbench_layout_node_set_split(
        &centre, UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL, 0.78);
    test_initialise_node(
        &right,
        "right",
        "Right Region",
        UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP);
    test_initialise_node(
        &project,
        "project",
        "Project Explorer",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    (void)umi_workbench_layout_node_set_component(
        &project, "umicom.studio.project-explorer", "org.umicom.studio");
    test_initialise_node(
        &editor,
        "editor",
        "Editor",
        UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP);
    (void)umi_workbench_layout_node_set_component(
        &editor, "umicom.studio.editor", "org.umicom.studio");
    test_initialise_node(
        &chat,
        "chat",
        "AI Chat",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    (void)umi_workbench_layout_node_set_component(
        &chat, "umicom.ai.chat", "org.umicom.studio");
    test_initialise_node(
        &output,
        "output",
        "Output",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    (void)umi_workbench_layout_node_set_component(
        &output, "umicom.studio.output", "org.umicom.studio");

    status = umi_workbench_layout_document_add_node(
        document, &root, &root_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &left, &left_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &centre, &centre_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &right, &right_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &project, &project_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &editor, &editor_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &chat, &chat_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &output, &output_index);
    }
    (void)root_index;
    (void)left_index;
    (void)centre_index;
    (void)right_index;
    (void)project_index;
    (void)editor_index;
    (void)chat_index;
    (void)output_index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_workbench_layout_document_set_root(document, "root");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "root", "left", 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "root", "centre", 1U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "root", "right", 2U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "left", "project", 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "centre", "editor", 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "centre", "output", 1U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, "right", "chat", 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    document->nodes[document->root_index].active_child_index = 1U;
    document->nodes[centre_index].active_child_index = 0U;
    document->nodes[left_index].active_child_index = 0U;
    document->nodes[right_index].active_child_index = 0U;
    document->audit.created_at_ms = 1000U;
    document->audit.modified_at_ms = 1000U;
    test_copy_text(
        document->audit.created_by,
        sizeof(document->audit.created_by),
        "developer");
    test_copy_text(
        document->audit.modified_by,
        sizeof(document->audit.modified_by),
        "developer");
    umi_workbench_layout_document_refresh_hash(document);
    return UMI_STATUS_OK;
}

/**
 * Exercise test allocate layout and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiWorkbenchLayoutDocument *test_allocate_layout(
    const char *layout_id)
{
    UmiWorkbenchLayoutDocument *document =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*document));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (test_create_development_layout(layout_id, document) !=
        UMI_STATUS_OK) {
        free(document);
        return NULL;
    }
    return document;
}

#endif
