/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/template_registry.c
 *
 * PURPOSE:
 *   Implement immutable Framework and application layout templates and clone them into user-owned semantic documents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/template_registry.h"

#include <string.h>

#include "internal.h"

/* Check that template satisfies its contract before another service relies on it. */
static UmiStatus template_validate(
    const UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout_template == NULL ||
        layout_template->structure_size < sizeof(*layout_template) ||
        !umi_workbench_layout_text_present(
            layout_template->template_id) ||
        !umi_workbench_layout_text_present(
            layout_template->display_name) ||
        !umi_workbench_layout_text_present(
            layout_template->category) ||
        layout_template->tag_count > UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_document_validate_structure(
        &layout_template->document);
    return status;
}

/* Provide the add node operation used by this module and its client applications. */
static UmiStatus add_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    UmiWorkbenchLayoutNodeKind kind,
    const char *title,
    const char *component_id,
    const char *owner_application_id,
    const char *parent_id,
    UmiWorkbenchLayoutOrientation orientation,
    double split_ratio,
    UmiWorkbenchLayoutDockRegion dock_region,
    size_t *out_index)
{
    UmiWorkbenchLayoutNode node;
    UmiStatus status;
    size_t node_index;

    umi_workbench_layout_node_init(&node, node_id, kind);
    node.orientation = orientation;
    node.split_ratio = split_ratio;
    node.dock_region = dock_region;
    node.visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
    node.flags =
        (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_CLOSABLE |
        (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_MOVABLE |
        (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (title != NULL && title[0] != '\0') {
        status = umi_workbench_layout_node_set_title(&node, title);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_id != NULL && component_id[0] != '\0') {
        status = umi_workbench_layout_node_set_component(
            &node,
            component_id,
            owner_application_id != NULL
                ? owner_application_id
                : "org.umicom.desktop");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = umi_workbench_layout_document_add_node(
        document, &node, &node_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (parent_id != NULL && parent_id[0] != '\0') {
        status = umi_workbench_layout_document_attach_child(
            document,
            parent_id,
            node_id,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index != NULL) {
        *out_index = node_index;
    }
    return UMI_STATUS_OK;
}

/* Provide the finish document operation used by this module and its client applications. */
static UmiStatus finish_document(
    UmiWorkbenchLayoutDocument *document,
    const char *root_id,
    const char *category,
    const char *description)
{
    UmiStatus status = umi_workbench_layout_document_set_root(
        document, root_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_metadata(
            document,
            document->name,
            category,
            description);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
            false);
    }
    return status;
}

/*
 * Provide the make blank template operation used by this module and its client
 * applications.
 */
static UmiStatus make_blank_template(
    UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiStatus status;

    (void)memset(layout_template, 0, sizeof(*layout_template));
    layout_template->structure_size = sizeof(*layout_template);
    (void)umi_workbench_layout_copy_text(
        layout_template->template_id,
        sizeof(layout_template->template_id),
        "framework.blank",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->display_name,
        sizeof(layout_template->display_name),
        "Blank Workbench",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->category,
        sizeof(layout_template->category),
        "framework",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->owner_application_id,
        sizeof(layout_template->owner_application_id),
        "org.umicom.desktop",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->description,
        sizeof(layout_template->description),
        "An empty semantic workbench ready for user-selected panels.",
        false);

    umi_workbench_layout_document_init(
        &layout_template->document,
        "framework.blank",
        "Blank Workbench");
    status = add_node(
        &layout_template->document,
        "blank.root",
        UMI_WORKBENCH_LAYOUT_NODE_EMPTY,
        "Empty Workbench",
        "",
        "",
        NULL,
        UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
        0.5,
        UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
        NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = finish_document(
            &layout_template->document,
            "blank.root",
            "framework",
            layout_template->description);
    }
    layout_template->built_in = true;
    layout_template->recommended = false;
    layout_template->revision = 1U;
    return status;
}

/*
 * Provide the make mosaic template operation used by this module and its client
 * applications.
 */
static UmiStatus make_mosaic_template(
    UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiWorkbenchLayoutDocument *document;
    UmiStatus status;

    (void)memset(layout_template, 0, sizeof(*layout_template));
    layout_template->structure_size = sizeof(*layout_template);
    (void)umi_workbench_layout_copy_text(
        layout_template->template_id,
        sizeof(layout_template->template_id),
        "framework.mosaic",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->display_name,
        sizeof(layout_template->display_name),
        "Mosaic",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->category,
        sizeof(layout_template->category),
        "desktop",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->owner_application_id,
        sizeof(layout_template->owner_application_id),
        "org.umicom.desktop",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->description,
        sizeof(layout_template->description),
        "Umicom Desk global bar, active workbench, application taskbar and layout strip.",
        false);

    document = &layout_template->document;
    umi_workbench_layout_document_init(
        document, "framework.mosaic", "Mosaic");

    status = add_node(
        document,
        "mosaic.root",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
        "Mosaic Root",
        "",
        "",
        NULL,
        UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
        0.08,
        UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
        NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.global-bar",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Global Command Bar",
            "umicom.desktop.global-command-bar",
            "org.umicom.desktop",
            "mosaic.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_TOP,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.main",
            UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
            "Main Workbench",
            "",
            "",
            "mosaic.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
            0.84,
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.active-workbench",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Active Workbench",
            "umicom.desktop.active-workbench",
            "org.umicom.desktop",
            "mosaic.main",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.bottom",
            UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
            "Desktop Strips",
            "",
            "",
            "mosaic.main",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
            0.50,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.application-strip",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Applications",
            "umicom.desktop.application-strip",
            "org.umicom.desktop",
            "mosaic.bottom",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "mosaic.layout-strip",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Layouts",
            "umicom.desktop.layout-strip",
            "org.umicom.desktop",
            "mosaic.bottom",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = finish_document(
            document,
            "mosaic.root",
            "desktop",
            layout_template->description);
    }
    layout_template->built_in = true;
    layout_template->recommended = true;
    layout_template->revision = 1U;
    return status;
}

/*
 * Provide the make development template operation used by this module and its client
 * applications.
 */
static UmiStatus make_development_template(
    UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiWorkbenchLayoutDocument *document;
    UmiStatus status;

    (void)memset(layout_template, 0, sizeof(*layout_template));
    layout_template->structure_size = sizeof(*layout_template);
    (void)umi_workbench_layout_copy_text(
        layout_template->template_id,
        sizeof(layout_template->template_id),
        "framework.development",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->display_name,
        sizeof(layout_template->display_name),
        "Development Workbench",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->category,
        sizeof(layout_template->category),
        "development",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->owner_application_id,
        sizeof(layout_template->owner_application_id),
        "org.umicom.studio",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->description,
        sizeof(layout_template->description),
        "Project Explorer, editor groups, contextual tools and diagnostics for software development.",
        false);

    document = &layout_template->document;
    umi_workbench_layout_document_init(
        document,
        "framework.development",
        "Development Workbench");

    status = add_node(
        document,
        "development.root",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
        "Development Root",
        "",
        "",
        NULL,
        UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL,
        0.20,
        UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
        NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.explorer",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Project Explorer",
            "umicom.studio.project-explorer",
            "org.umicom.studio",
            "development.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_LEFT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.centre-right",
            UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
            "Editor and Tools",
            "",
            "",
            "development.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL,
            0.72,
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.centre",
            UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
            "Editor and Output",
            "",
            "",
            "development.centre-right",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
            0.76,
            UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.editors",
            UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP,
            "Editors",
            "umicom.studio.editor-group",
            "org.umicom.studio",
            "development.centre",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.output-tabs",
            UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP,
            "Output and Problems",
            "",
            "",
            "development.centre",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.output",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Output",
            "umicom.studio.output",
            "org.umicom.studio",
            "development.output-tabs",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.problems",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Problems",
            "umicom.studio.problems",
            "org.umicom.studio",
            "development.output-tabs",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.tools",
            UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP,
            "Context Tools",
            "",
            "",
            "development.centre-right",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_RIGHT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.chat",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Chat",
            "umicom.studio.chat",
            "org.umicom.studio",
            "development.tools",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_RIGHT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "development.outline",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Outline",
            "umicom.studio.outline",
            "org.umicom.studio",
            "development.tools",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_RIGHT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = finish_document(
            document,
            "development.root",
            "development",
            layout_template->description);
    }
    layout_template->built_in = true;
    layout_template->recommended = true;
    layout_template->revision = 1U;
    return status;
}

/*
 * Provide the make operations template operation used by this module and its client
 * applications.
 */
static UmiStatus make_operations_template(
    UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiWorkbenchLayoutDocument *document;
    UmiStatus status;

    (void)memset(layout_template, 0, sizeof(*layout_template));
    layout_template->structure_size = sizeof(*layout_template);
    (void)umi_workbench_layout_copy_text(
        layout_template->template_id,
        sizeof(layout_template->template_id),
        "framework.operations",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->display_name,
        sizeof(layout_template->display_name),
        "Operations Workbench",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->category,
        sizeof(layout_template->category),
        "operations",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->owner_application_id,
        sizeof(layout_template->owner_application_id),
        "org.umicom.desktop",
        false);
    (void)umi_workbench_layout_copy_text(
        layout_template->description,
        sizeof(layout_template->description),
        "System health, event activity, diagnostics and supervised task state.",
        false);

    document = &layout_template->document;
    umi_workbench_layout_document_init(
        document,
        "framework.operations",
        "Operations Workbench");

    status = add_node(
        document,
        "operations.root",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
        "Operations Root",
        "",
        "",
        NULL,
        UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
        0.62,
        UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
        NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.overview",
            UMI_WORKBENCH_LAYOUT_NODE_SPLIT,
            "Overview",
            "",
            "",
            "operations.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL,
            0.50,
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.health",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "System Health",
            "umicom.operations.health",
            "org.umicom.desktop",
            "operations.overview",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_LEFT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.tasks",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Supervised Tasks",
            "umicom.operations.tasks",
            "org.umicom.desktop",
            "operations.overview",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_RIGHT,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.events",
            UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP,
            "Operational Evidence",
            "",
            "",
            "operations.root",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.event-log",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Event Log",
            "umicom.operations.event-log",
            "org.umicom.desktop",
            "operations.events",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_node(
            document,
            "operations.diagnostics",
            UMI_WORKBENCH_LAYOUT_NODE_PANEL,
            "Diagnostics",
            "umicom.operations.diagnostics",
            "org.umicom.desktop",
            "operations.events",
            UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE,
            0.5,
            UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM,
            NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = finish_document(
            document,
            "operations.root",
            "operations",
            layout_template->description);
    }
    layout_template->built_in = true;
    layout_template->recommended = false;
    layout_template->revision = 1U;
    return status;
}

/*
 * Initialise workbench layout template registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_template_registry_init(
    UmiWorkbenchLayoutTemplateRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return;
    }
    (void)memset(registry, 0, sizeof(*registry));
    registry->structure_size = sizeof(*registry);
    registry->revision = 1U;
}

/*
 * Add workbench layout template registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_layout_template_registry_add(
    UmiWorkbenchLayoutTemplateRegistry *registry,
    const UmiWorkbenchLayoutTemplate *layout_template)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || layout_template == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = template_validate(layout_template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_WORKBENCH_LAYOUT_MAX_TEMPLATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_template_registry_find(
            registry, layout_template->template_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    registry->templates[registry->count] = *layout_template;
    registry->templates[registry->count].structure_size =
        sizeof(registry->templates[registry->count]);
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench layout template registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_layout_template_registry_remove(
    UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(template_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->templates[index].template_id,
                template_id) != 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->templates[index].built_in) {
            return UMI_STATUS_PERMISSION_DENIED;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (; index + 1U < registry->count; ++index) {
            registry->templates[index] =
                registry->templates[index + 1U];
        }
        registry->count -= 1U;
        (void)memset(
            &registry->templates[registry->count],
            0,
            sizeof(registry->templates[registry->count]));
        registry->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find workbench layout template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutTemplate *
umi_workbench_layout_template_registry_find(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(template_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->templates[index].template_id,
                template_id) == 0) {
            return &registry->templates[index];
        }
    }
    return NULL;
}

/*
 * Find workbench layout template registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutTemplate *
umi_workbench_layout_template_registry_at(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }
    return &registry->templates[index];
}

/*
 * Provide the workbench layout template registry clone operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_template_registry_clone(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *template_id,
    const UmiWorkbenchLayoutIdentity *identity,
    const char *name,
    UmiWorkbenchLayoutDocument *out_document)
{
    const UmiWorkbenchLayoutTemplate *layout_template;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || identity == NULL ||
        out_document == NULL ||
        !umi_workbench_layout_text_present(template_id) ||
        !umi_workbench_layout_text_present(identity->layout_id) ||
        !umi_workbench_layout_text_present(name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    layout_template =
        umi_workbench_layout_template_registry_find(
            registry, template_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout_template == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_workbench_layout_document_copy(
        out_document, &layout_template->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_identity(
            out_document, identity);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            out_document->name,
            sizeof(out_document->name),
            name,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        out_document->flags &=
            ~((uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN |
              (uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED |
              (uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY);
        out_document->flags |=
            (uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
        out_document->version.base_revision = 0U;
        out_document->version.revision = 1U;
        out_document->version.generation += 1U;
        out_document->audit.created_at_ms = 0U;
        out_document->audit.modified_at_ms = 0U;
        out_document->audit.created_by[0] = '\0';
        out_document->audit.modified_by[0] = '\0';
        umi_workbench_layout_document_refresh_hash(out_document);
    }
    return status;
}

/*
 * Provide the workbench layout template registry count category operation used by this
 * module and its client applications.
 */
size_t umi_workbench_layout_template_registry_count_category(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *category)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(category)) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->templates[index].category,
                category) == 0) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench layout template registry count owner operation used by this module
 * and its client applications.
 */
size_t umi_workbench_layout_template_registry_count_owner(
    const UmiWorkbenchLayoutTemplateRegistry *registry,
    const char *owner_application_id)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(owner_application_id)) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->templates[index].owner_application_id,
                owner_application_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench layout template registry seed framework operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_layout_template_registry_seed_framework(
    UmiWorkbenchLayoutTemplateRegistry *registry)
{
    UmiWorkbenchLayoutTemplate layout_template;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = make_blank_template(&layout_template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_template_registry_find(
            registry, layout_template.template_id) == NULL) {
        status = umi_workbench_layout_template_registry_add(
            registry, &layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = make_mosaic_template(&layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_template_registry_find(
            registry, layout_template.template_id) == NULL) {
        status = umi_workbench_layout_template_registry_add(
            registry, &layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = make_development_template(&layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_template_registry_find(
            registry, layout_template.template_id) == NULL) {
        status = umi_workbench_layout_template_registry_add(
            registry, &layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = make_operations_template(&layout_template);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        umi_workbench_layout_template_registry_find(
            registry, layout_template.template_id) == NULL) {
        status = umi_workbench_layout_template_registry_add(
            registry, &layout_template);
    }
    return status;
}
