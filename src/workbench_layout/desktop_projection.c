/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/desktop_projection.c
 *
 * PURPOSE:
 *   Translate persistent semantic layout documents to and from the existing flat desktop window model without introducing GTK dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/desktop_projection.h"

#include <string.h>

#include "internal.h"

static UmiDesktopDockPlacement map_dock_region(
    UmiWorkbenchLayoutDockRegion region)
{
    switch (region) {
    case UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT:
        return UMI_DESKTOP_DOCK_DOCUMENT;
    case UMI_WORKBENCH_LAYOUT_DOCK_LEFT:
        return UMI_DESKTOP_DOCK_LEFT;
    case UMI_WORKBENCH_LAYOUT_DOCK_RIGHT:
        return UMI_DESKTOP_DOCK_RIGHT;
    case UMI_WORKBENCH_LAYOUT_DOCK_TOP:
        return UMI_DESKTOP_DOCK_TOP;
    case UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM:
        return UMI_DESKTOP_DOCK_BOTTOM;
    case UMI_WORKBENCH_LAYOUT_DOCK_FLOATING:
        return UMI_DESKTOP_DOCK_FLOATING;
    case UMI_WORKBENCH_LAYOUT_DOCK_CANVAS:
    default:
        return UMI_DESKTOP_DOCK_CANVAS;
    }
}

static UmiWorkbenchLayoutDockRegion map_desktop_dock(
    UmiDesktopDockPlacement placement)
{
    switch (placement) {
    case UMI_DESKTOP_DOCK_DOCUMENT:
        return UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT;
    case UMI_DESKTOP_DOCK_LEFT:
        return UMI_WORKBENCH_LAYOUT_DOCK_LEFT;
    case UMI_DESKTOP_DOCK_RIGHT:
        return UMI_WORKBENCH_LAYOUT_DOCK_RIGHT;
    case UMI_DESKTOP_DOCK_TOP:
        return UMI_WORKBENCH_LAYOUT_DOCK_TOP;
    case UMI_DESKTOP_DOCK_BOTTOM:
        return UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM;
    case UMI_DESKTOP_DOCK_FLOATING:
        return UMI_WORKBENCH_LAYOUT_DOCK_FLOATING;
    case UMI_DESKTOP_DOCK_CANVAS:
    default:
        return UMI_WORKBENCH_LAYOUT_DOCK_CANVAS;
    }
}

static bool should_project_node(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutProjectionOptions *options,
    bool *out_hidden,
    bool *out_container)
{
    *out_hidden = false;
    *out_container = false;

    if (node->visibility ==
            UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN &&
        !options->include_hidden_nodes) {
        *out_hidden = true;
        return false;
    }
    if (umi_workbench_layout_node_is_container(node) &&
        node->kind != UMI_WORKBENCH_LAYOUT_NODE_WINDOW &&
        node->kind !=
            UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW &&
        !options->include_container_windows) {
        *out_container = true;
        return false;
    }
    return node->component_id[0] != '\0' ||
           node->kind == UMI_WORKBENCH_LAYOUT_NODE_WINDOW ||
           node->kind ==
               UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
}

static UmiStatus project_one(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutProjectionOptions *options,
    size_t index,
    UmiDesktopWindow *window)
{
    UmiStatus status;

    (void)memset(window, 0, sizeof(*window));
    status = umi_workbench_layout_copy_text(
        window->window_id,
        sizeof(window->window_id),
        node->node_id,
        false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            window->title,
            sizeof(window->title),
            node->title[0] != '\0'
                ? node->title
                : node->node_id,
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            window->component_id,
            sizeof(window->component_id),
            node->component_id,
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            window->owner_application_id,
            sizeof(window->owner_application_id),
            node->owner_application_id,
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            window->monitor_id,
            sizeof(window->monitor_id),
            node->monitor_id[0] != '\0'
                ? node->monitor_id
                : options->default_monitor_id,
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            window->context_group_id,
            sizeof(window->context_group_id),
            node->context_group_id,
            true);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    window->bounds.x = node->bounds.x;
    window->bounds.y = node->bounds.y;
    window->bounds.width =
        node->bounds.width > 0
            ? node->bounds.width
            : options->default_bounds.width;
    window->bounds.height =
        node->bounds.height > 0
            ? node->bounds.height
            : options->default_bounds.height;
    window->dock_placement =
        map_dock_region(node->dock_region);
    window->z_order = options->preserve_z_order
        ? node->z_order
        : (int32_t)index;
    window->visible =
        node->visibility !=
            UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    window->maximised = false;
    window->closable =
        umi_workbench_layout_node_has_flag(
            node, UMI_WORKBENCH_LAYOUT_NODE_CLOSABLE);
    window->resizable =
        umi_workbench_layout_node_has_flag(
            node, UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE);
    return UMI_STATUS_OK;
}

UmiWorkbenchLayoutProjectionOptions
umi_workbench_layout_projection_options_default(void)
{
    UmiWorkbenchLayoutProjectionOptions options;

    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    (void)umi_workbench_layout_copy_text(
        options.default_monitor_id,
        sizeof(options.default_monitor_id),
        "primary",
        false);
    options.default_bounds.x = 40;
    options.default_bounds.y = 40;
    options.default_bounds.width = 900;
    options.default_bounds.height = 640;
    options.include_hidden_nodes = false;
    options.include_container_windows = false;
    options.preserve_z_order = true;
    return options;
}

UmiStatus umi_workbench_layout_project_desktop(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutProjectionOptions *options,
    UmiDesktopLayout *out_layout,
    UmiWorkbenchLayoutProjectionReport *out_report)
{
    UmiWorkbenchLayoutProjectionOptions effective;
    UmiWorkbenchLayoutProjectionReport report;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;

    if (document == NULL || out_layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective = options != NULL
        ? *options
        : umi_workbench_layout_projection_options_default();
    (void)memset(out_layout, 0, sizeof(*out_layout));
    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    report.complete = true;

    (void)umi_workbench_layout_copy_text(
        out_layout->layout_id,
        sizeof(out_layout->layout_id),
        document->identity.layout_id,
        false);
    (void)umi_workbench_layout_copy_text(
        out_layout->name,
        sizeof(out_layout->name),
        document->name,
        false);
    (void)umi_workbench_layout_copy_text(
        out_layout->category,
        sizeof(out_layout->category),
        document->category,
        true);
    (void)umi_workbench_layout_copy_text(
        out_layout->description,
        sizeof(out_layout->description),
        document->description,
        true);
    out_layout->built_in =
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN);
    out_layout->locked =
        umi_workbench_layout_document_has_flag(
            document,
            UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    out_layout->revision = document->version.revision;

    for (index = 0U;
         index < document->node_count &&
         status == UMI_STATUS_OK;
         ++index) {
        bool hidden;
        bool container;

        report.inspected_node_count += 1U;
        if (!should_project_node(
                &document->nodes[index],
                &effective,
                &hidden,
                &container)) {
            if (hidden) {
                report.skipped_hidden_count += 1U;
            }
            if (container) {
                report.skipped_container_count += 1U;
            }
            continue;
        }
        if (out_layout->window_count >=
            UMI_DESKTOP_MAX_LAYOUT_WINDOWS) {
            report.truncated_count += 1U;
            report.complete = false;
            continue;
        }

        status = project_one(
            &document->nodes[index],
            &effective,
            out_layout->window_count,
            &out_layout->windows[out_layout->window_count]);
        if (status == UMI_STATUS_OK) {
            out_layout->window_count += 1U;
            report.projected_window_count += 1U;
        }
    }

    if (out_report != NULL) {
        *out_report = report;
    }
    return status;
}

UmiStatus umi_workbench_layout_import_desktop(
    const UmiDesktopLayout *desktop_layout,
    const UmiWorkbenchLayoutIdentity *identity,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutProjectionReport *out_report)
{
    UmiWorkbenchLayoutProjectionReport report;
    UmiWorkbenchLayoutNode root;
    size_t root_index;
    size_t index;
    UmiStatus status;

    if (desktop_layout == NULL || identity == NULL ||
        out_document == NULL ||
        !umi_workbench_layout_text_present(
            desktop_layout->layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    report.complete = true;
    umi_workbench_layout_document_init(
        out_document,
        identity->layout_id,
        desktop_layout->name);
    status = umi_workbench_layout_document_set_identity(
        out_document, identity);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_metadata(
            out_document,
            desktop_layout->name,
            desktop_layout->category,
            desktop_layout->description);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_workbench_layout_node_init(
        &root,
        "desktop-import.root",
        UMI_WORKBENCH_LAYOUT_NODE_WINDOW);
    (void)umi_workbench_layout_node_set_title(
        &root, desktop_layout->name);
    root.flags =
        (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_PRIMARY |
        (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE;
    status = umi_workbench_layout_document_add_node(
        out_document, &root, &root_index);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_set_root(
            out_document, root.node_id);
    }

    for (index = 0U;
         index < desktop_layout->window_count &&
         status == UMI_STATUS_OK;
         ++index) {
        const UmiDesktopWindow *window =
            &desktop_layout->windows[index];
        UmiWorkbenchLayoutNode node;
        size_t node_index;

        umi_workbench_layout_node_init(
            &node,
            window->window_id,
            window->dock_placement == UMI_DESKTOP_DOCK_FLOATING
                ? UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW
                : UMI_WORKBENCH_LAYOUT_NODE_PANEL);
        (void)umi_workbench_layout_node_set_title(
            &node, window->title);
        if (window->component_id[0] != '\0') {
            (void)umi_workbench_layout_node_set_component(
                &node,
                window->component_id,
                window->owner_application_id);
        }
        (void)umi_workbench_layout_node_set_context_group(
            &node, window->context_group_id);
        (void)umi_workbench_layout_node_set_monitor(
            &node, window->monitor_id);
        node.bounds.x = window->bounds.x;
        node.bounds.y = window->bounds.y;
        node.bounds.width = window->bounds.width;
        node.bounds.height = window->bounds.height;
        node.dock_region =
            map_desktop_dock(window->dock_placement);
        node.visibility = window->visible
            ? UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE
            : UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
        node.z_order = window->z_order;
        node.flags = 0U;
        if (window->closable) {
            node.flags |=
                (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_CLOSABLE;
        }
        if (window->resizable) {
            node.flags |=
                (uint32_t)UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE;
        }

        status = umi_workbench_layout_document_add_node(
            out_document, &node, &node_index);
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_document_attach_child(
                out_document,
                root.node_id,
                node.node_id,
                UMI_WORKBENCH_LAYOUT_INDEX_NONE);
        }
        if (status == UMI_STATUS_OK) {
            report.projected_window_count += 1U;
        }
        report.inspected_node_count += 1U;
        (void)node_index;
    }

    if (status == UMI_STATUS_OK) {
        out_document->flags =
            desktop_layout->built_in
                ? (uint32_t)
                    UMI_WORKBENCH_LAYOUT_DOCUMENT_BUILT_IN
                : 0U;
        if (desktop_layout->locked) {
            out_document->flags |=
                (uint32_t)
                    UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED;
        }
        out_document->version.revision =
            desktop_layout->revision > 0U
                ? desktop_layout->revision
                : 1U;
        umi_workbench_layout_document_refresh_hash(
            out_document);
    }
    if (out_report != NULL) {
        *out_report = report;
    }
    return status;
}
