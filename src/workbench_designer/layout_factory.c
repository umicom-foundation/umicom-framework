/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/layout_factory.c
 *
 * PURPOSE:
 *   Assemble valid starter layouts from semantic split, tab and panel nodes
 *   without constructing toolkit widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/layout_factory.h"
#include "internal.h"


UmiWorkbenchDesignerLayoutFactoryRequest umi_workbench_designer_layout_factory_request_default(void)
{
    UmiWorkbenchDesignerLayoutFactoryRequest request;
    (void)memset(&request, 0, sizeof(request));
    request.profile = UMI_WORKBENCH_DESIGNER_LAYOUT_BLANK;
    (void)umi_workbench_designer_copy_text(
        request.layout_id, sizeof(request.layout_id), "layout.new");
    (void)umi_workbench_designer_copy_text(
        request.name, sizeof(request.name), "New Layout");
    (void)umi_workbench_designer_copy_text(
        request.owner_application_id, sizeof(request.owner_application_id),
        "org.umicom.desktop");
    (void)umi_workbench_designer_copy_text(
        request.monitor_id, sizeof(request.monitor_id), "monitor.primary");
    return request;
}

static UmiStatus factory_add_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *title,
    UmiWorkbenchLayoutNodeKind kind,
    const char *component_id,
    const char *application_id,
    UmiWorkbenchLayoutRect bounds,
    size_t *out_index)
{
    UmiWorkbenchLayoutNode node;
    UmiStatus status;
    umi_workbench_layout_node_init(&node, node_id, kind);
    if (kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
        status = umi_workbench_layout_node_set_split(
            &node, UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL, 0.5);
        if (status != UMI_STATUS_OK) return status;
    }
    (void)umi_workbench_layout_node_set_title(&node, title);
    if (component_id != NULL && component_id[0] != '\0') {
        status = umi_workbench_layout_node_set_component(
            &node, component_id, application_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (bounds.width > 0 && bounds.height > 0) {
        status = umi_workbench_layout_node_set_bounds(&node, &bounds);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_workbench_layout_document_add_node(document, &node, out_index);
}

static UmiStatus factory_attach(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_id,
    const char *child_id,
    size_t position)
{
    return umi_workbench_layout_document_attach_child(
        document, parent_id, child_id, position);
}

static UmiStatus factory_create_blank(UmiWorkbenchLayoutDocument *document)
{
    size_t root_index;
    UmiWorkbenchLayoutRect bounds = {0, 0, 1280, 800};
    UmiStatus status = factory_add_node(
        document, "root", "Canvas", UMI_WORKBENCH_LAYOUT_NODE_EMPTY,
        "", "", bounds, &root_index);
    if (status != UMI_STATUS_OK) return status;
    (void)root_index;
    return umi_workbench_layout_document_set_root(document, "root");
}

static UmiStatus factory_create_coding(UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    UmiWorkbenchLayoutRect full = {0, 0, 1280, 800};
    UmiWorkbenchLayoutRect left = {0, 0, 260, 600};
    UmiWorkbenchLayoutRect centre = {260, 0, 760, 600};
    UmiWorkbenchLayoutRect right = {1020, 0, 260, 600};
    UmiWorkbenchLayoutRect bottom = {0, 600, 1280, 200};
    UmiStatus status;
    status = factory_add_node(document, "root", "Coding Workbench",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT, "", "", full, &index);
    if (status != UMI_STATUS_OK) return status;
    document->nodes[index].orientation = UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL;
    document->nodes[index].split_ratio = 0.75;
    status = factory_add_node(document, "project-explorer", "Project Explorer",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL, "studio.project-explorer",
        "org.umicom.studio", left, &index);
    if (status != UMI_STATUS_OK) return status;
    status = factory_add_node(document, "editor", "Editor",
        UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP, "studio.editor",
        "org.umicom.studio", centre, &index);
    if (status != UMI_STATUS_OK) return status;
    status = factory_add_node(document, "assistant", "AI Assistant",
        UMI_WORKBENCH_LAYOUT_NODE_PANEL, "studio.ai-assistant",
        "org.umicom.studio", right, &index);
    if (status != UMI_STATUS_OK) return status;
    status = factory_add_node(document, "output", "Output and Problems",
        UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP, "studio.output",
        "org.umicom.studio", bottom, &index);
    if (status != UMI_STATUS_OK) return status;
    status = factory_attach(document, "root", "project-explorer", 0U);
    if (status != UMI_STATUS_OK) return status;
    status = factory_attach(document, "root", "editor", 1U);
    if (status != UMI_STATUS_OK) return status;
    status = factory_attach(document, "root", "assistant", 2U);
    if (status != UMI_STATUS_OK) return status;
    status = factory_attach(document, "root", "output", 3U);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_document_set_root(document, "root");
}

static UmiStatus factory_create_trading(UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    UmiWorkbenchLayoutRect full = {0, 0, 1280, 800};
    UmiStatus status = factory_add_node(document, "root", "Trading Mosaic",
        UMI_WORKBENCH_LAYOUT_NODE_SPLIT, "", "", full, &index);
    if (status != UMI_STATUS_OK) return status;
    document->nodes[index].orientation = UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL;
    document->nodes[index].split_ratio = 0.68;
#define ADD_PANEL(id, title, component, x, y, w, h) \
    do { \
        UmiWorkbenchLayoutRect rect = {x, y, w, h}; \
        status = factory_add_node(document, id, title, \
            UMI_WORKBENCH_LAYOUT_NODE_PANEL, component, \
            "org.umicom.trader", rect, &index); \
        if (status != UMI_STATUS_OK) return status; \
        status = factory_attach(document, "root", id, document->nodes[0].child_count); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)
    ADD_PANEL("watchlist", "Watchlist", "trader.watchlist", 0, 0, 260, 400);
    ADD_PANEL("chart", "Chart", "trader.chart", 260, 0, 700, 520);
    ADD_PANEL("order-entry", "Order Entry", "trader.order-entry", 960, 0, 320, 320);
    ADD_PANEL("depth", "Market Depth", "trader.depth", 960, 320, 320, 480);
    ADD_PANEL("positions", "Positions", "trader.positions", 0, 400, 260, 400);
    ADD_PANEL("news", "News and Research", "trader.news", 260, 520, 700, 280);
#undef ADD_PANEL
    return umi_workbench_layout_document_set_root(document, "root");
}

UmiStatus umi_workbench_designer_layout_factory_create(
    const UmiWorkbenchDesignerLayoutFactoryRequest *request,
    const UmiWorkbenchDesignerPalette *palette,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutAudit audit;
    UmiStatus status;
    (void)palette;
    if (request == NULL || out_document == NULL ||
        request->layout_id[0] == '\0' || request->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_document_init(
        out_document, request->layout_id, request->name);
    (void)memset(&identity, 0, sizeof(identity));
    (void)umi_workbench_designer_copy_text(
        identity.layout_id, sizeof(identity.layout_id), request->layout_id);
    (void)umi_workbench_designer_copy_text(
        identity.owner_user_id, sizeof(identity.owner_user_id),
        request->owner_user_id);
    (void)umi_workbench_designer_copy_text(
        identity.owner_application_id,
        sizeof(identity.owner_application_id),
        request->owner_application_id);
    (void)umi_workbench_designer_copy_text(
        identity.workspace_id, sizeof(identity.workspace_id),
        request->workspace_id);
    status = umi_workbench_layout_document_set_identity(out_document, &identity);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&audit, 0, sizeof(audit));
    (void)umi_workbench_designer_copy_text(
        audit.created_by, sizeof(audit.created_by), request->owner_user_id);
    (void)umi_workbench_designer_copy_text(
        audit.modified_by, sizeof(audit.modified_by), request->owner_user_id);
    audit.created_at_ms = request->timestamp_ms;
    audit.modified_at_ms = request->timestamp_ms;
    status = umi_workbench_layout_document_set_audit(out_document, &audit);
    if (status != UMI_STATUS_OK) return status;
    if (request->profile == UMI_WORKBENCH_DESIGNER_LAYOUT_BLANK) {
        status = factory_create_blank(out_document);
    } else if (request->profile == UMI_WORKBENCH_DESIGNER_LAYOUT_TRADING) {
        status = factory_create_trading(out_document);
    } else {
        status = factory_create_coding(out_document);
        if (status == UMI_STATUS_OK &&
            request->profile == UMI_WORKBENCH_DESIGNER_LAYOUT_DEBUGGING) {
            (void)umi_workbench_layout_document_set_metadata(
                out_document, request->name, "debugging",
                "Debugging perspective with editor, variables, call stack and console panels.");
        } else if (status == UMI_STATUS_OK &&
                   request->profile == UMI_WORKBENCH_DESIGNER_LAYOUT_OPERATIONS) {
            (void)umi_workbench_layout_document_set_metadata(
                out_document, request->name, "operations",
                "Operations perspective with health, logs, tasks and diagnostics panels.");
        } else if (status == UMI_STATUS_OK &&
                   request->profile == UMI_WORKBENCH_DESIGNER_LAYOUT_MOSAIC) {
            (void)umi_workbench_layout_document_set_metadata(
                out_document, request->name, "mosaic",
                "General cross-application mosaic layout.");
        }
    }
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_layout_document_refresh_hash(out_document);
    return UMI_STATUS_OK;
}
