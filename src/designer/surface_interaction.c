/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/surface_interaction.c
 *
 * PURPOSE:
 *   Implement safe hit-testing and undoable direct manipulation for reusable
 *   visual-design surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/surface_interaction.h"

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Convert a wide intermediate result only after keeping it inside C's int32 range. */
static int32_t clamp_i64_to_i32(int64_t value)
{
    if (value < INT32_MIN) return INT32_MIN;
    if (value > INT32_MAX) return INT32_MAX;
    return (int32_t)value;
}

/* Compare rectangles by value so a click without movement creates no history entry. */
static int rect_equal(UmiDesignerRect left, UmiDesignerRect right)
{
    return left.x == right.x && left.y == right.y &&
           left.width == right.width && left.height == right.height;
}

/* Include the right and bottom borders because those pixels contain resize handles. */
static int point_in_rect(int32_t x, int32_t y, UmiDesignerRect rect)
{
    int64_t right = (int64_t)rect.x + (int64_t)rect.width;
    int64_t bottom = (int64_t)rect.y + (int64_t)rect.height;

    return rect.width >= 0 && rect.height >= 0 &&
           (int64_t)x >= rect.x && (int64_t)x <= right &&
           (int64_t)y >= rect.y && (int64_t)y <= bottom;
}

/* Report every edge within the handle band so corner resizing works naturally. */
static unsigned hit_edges(
    int32_t x,
    int32_t y,
    UmiDesignerRect rect,
    int32_t handle_size)
{
    int64_t right = (int64_t)rect.x + (int64_t)rect.width;
    int64_t bottom = (int64_t)rect.y + (int64_t)rect.height;
    unsigned edges = UMI_DESIGNER_SURFACE_EDGE_NONE;

    if (handle_size <= 0) return edges;
    if ((int64_t)x - rect.x <= handle_size) {
        edges |= UMI_DESIGNER_SURFACE_EDGE_LEFT;
    }
    if (right - (int64_t)x <= handle_size) {
        edges |= UMI_DESIGNER_SURFACE_EDGE_RIGHT;
    }
    if ((int64_t)y - rect.y <= handle_size) {
        edges |= UMI_DESIGNER_SURFACE_EDGE_TOP;
    }
    if (bottom - (int64_t)y <= handle_size) {
        edges |= UMI_DESIGNER_SURFACE_EDGE_BOTTOM;
    }
    /* A very small imported component can place both opposite edges inside
     * the handle band. Choosing the nearest one avoids an ambiguous resize. */
    if ((edges & UMI_DESIGNER_SURFACE_EDGE_LEFT) != 0U &&
        (edges & UMI_DESIGNER_SURFACE_EDGE_RIGHT) != 0U) {
        edges &= ~((unsigned)UMI_DESIGNER_SURFACE_EDGE_LEFT |
                   (unsigned)UMI_DESIGNER_SURFACE_EDGE_RIGHT);
        edges |= (unsigned)(
            (int64_t)x - rect.x <= right - (int64_t)x
                ? UMI_DESIGNER_SURFACE_EDGE_LEFT
                : UMI_DESIGNER_SURFACE_EDGE_RIGHT);
    }
    if ((edges & UMI_DESIGNER_SURFACE_EDGE_TOP) != 0U &&
        (edges & UMI_DESIGNER_SURFACE_EDGE_BOTTOM) != 0U) {
        edges &= ~((unsigned)UMI_DESIGNER_SURFACE_EDGE_TOP |
                   (unsigned)UMI_DESIGNER_SURFACE_EDGE_BOTTOM);
        edges |= (unsigned)(
            (int64_t)y - rect.y <= bottom - (int64_t)y
                ? UMI_DESIGNER_SURFACE_EDGE_TOP
                : UMI_DESIGNER_SURFACE_EDGE_BOTTOM);
    }
    return edges;
}

/* Search from the top of the draw order and return an independent hit record. */
UmiStatus umi_designer_surface_hit_test(
    const UmiDesignerDocument *document,
    int32_t x,
    int32_t y,
    int32_t handle_size,
    UmiDesignerSurfaceHit *out_hit)
{
    UmiDesignerDocumentSnapshot snapshot;
    UmiDeclDocument *declarative;
    size_t index;
    UmiStatus status;

    if (document == NULL || out_hit == NULL || handle_size < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_hit, 0, sizeof(*out_hit));
    out_hit->structure_size = (uint32_t)sizeof(*out_hit);
    out_hit->api_version = UMI_DESIGNER_SURFACE_INTERACTION_API_VERSION;
    status = umi_designer_document_snapshot(document, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    declarative = umi_designer_document_declarative(
        (UmiDesignerDocument *)document);

    /* Later document nodes are drawn later, so searching backwards returns
     * the same component a user sees on top of an overlap. */
    for (index = snapshot.component_count; index > 0U; --index) {
        UmiDeclNode node;
        UmiDesignerRect rect;

        status = umi_decl_document_node_at(declarative, index - 1U, &node);
        if (status != UMI_STATUS_OK) return status;
        status = umi_designer_surface_get_rect(document, node.node_id, &rect);
        /* Containers and metadata nodes may intentionally have no rectangle. */
        if (status == UMI_STATUS_NOT_FOUND) continue;
        if (status != UMI_STATUS_OK) return status;
        if (!point_in_rect(x, y, rect)) continue;
        status = umi_decl_copy_text(
            out_hit->node_id,
            sizeof(out_hit->node_id),
            node.node_id);
        if (status != UMI_STATUS_OK) return status;
        out_hit->rect = rect;
        out_hit->edges = hit_edges(x, y, rect, handle_size);
        out_hit->found = 1;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Capture immutable starting geometry for a move or resize preview. */
UmiStatus umi_designer_surface_interaction_begin(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerSurfaceInteractionKind kind,
    unsigned edges,
    int32_t pointer_x,
    int32_t pointer_y,
    UmiDesignerRect canvas_bounds,
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerSurfaceInteraction *out_interaction)
{
    UmiStatus status;

    /* Reject incomplete sessions and manually assembled options that bypassed
     * the public grid and zoom setters. */
    if (document == NULL || !umi_decl_id_is_valid(node_id) ||
        out_interaction == NULL ||
        (kind != UMI_DESIGNER_SURFACE_INTERACTION_MOVE &&
         kind != UMI_DESIGNER_SURFACE_INTERACTION_RESIZE) ||
        (kind == UMI_DESIGNER_SURFACE_INTERACTION_MOVE && edges != 0U) ||
        (kind == UMI_DESIGNER_SURFACE_INTERACTION_RESIZE && edges == 0U) ||
        (edges & ~((unsigned)UMI_DESIGNER_SURFACE_EDGE_LEFT |
                   (unsigned)UMI_DESIGNER_SURFACE_EDGE_TOP |
                   (unsigned)UMI_DESIGNER_SURFACE_EDGE_RIGHT |
                   (unsigned)UMI_DESIGNER_SURFACE_EDGE_BOTTOM)) != 0U ||
        (options != NULL &&
         (options->grid_size == 0U || options->grid_size > 256U ||
          !(options->zoom >= 0.25F && options->zoom <= 4.0F)))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_interaction, 0, sizeof(*out_interaction));
    out_interaction->structure_size = (uint32_t)sizeof(*out_interaction);
    out_interaction->api_version =
        UMI_DESIGNER_SURFACE_INTERACTION_API_VERSION;
    status = umi_decl_copy_text(
        out_interaction->node_id,
        sizeof(out_interaction->node_id),
        node_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_surface_get_rect(
        document,
        node_id,
        &out_interaction->before);
    if (status != UMI_STATUS_OK) return status;

    out_interaction->kind = kind;
    out_interaction->edges = edges;
    out_interaction->canvas_bounds = canvas_bounds;
    out_interaction->preview = out_interaction->before;
    out_interaction->pointer_start_x = pointer_x;
    out_interaction->pointer_start_y = pointer_y;
    out_interaction->minimum_width =
        UMI_DESIGNER_SURFACE_DEFAULT_MINIMUM_SIZE;
    out_interaction->minimum_height =
        UMI_DESIGNER_SURFACE_DEFAULT_MINIMUM_SIZE;
    if (options != NULL) {
        out_interaction->options = *options;
    } else {
        umi_designer_surface_options_init(&out_interaction->options);
    }
    /* Very small preview surfaces still remain usable by reducing the normal
     * minimum size to the available canvas dimension. */
    if (canvas_bounds.width > 0 &&
        canvas_bounds.width < out_interaction->minimum_width) {
        out_interaction->minimum_width = canvas_bounds.width;
    }
    if (canvas_bounds.height > 0 &&
        canvas_bounds.height < out_interaction->minimum_height) {
        out_interaction->minimum_height = canvas_bounds.height;
    }
    out_interaction->active = 1;
    return UMI_STATUS_OK;
}

/* Keep a moved component inside the usable canvas without changing its size. */
static void constrain_move(
    UmiDesignerRect bounds,
    UmiDesignerRect *rect)
{
    int64_t maximum;

    if (bounds.width > 0) {
        maximum = (int64_t)bounds.x + bounds.width - rect->width;
        if (maximum < bounds.x) maximum = bounds.x;
        if (rect->x < bounds.x) rect->x = bounds.x;
        if ((int64_t)rect->x > maximum) {
            rect->x = clamp_i64_to_i32(maximum);
        }
    }
    if (bounds.height > 0) {
        maximum = (int64_t)bounds.y + bounds.height - rect->height;
        if (maximum < bounds.y) maximum = bounds.y;
        if (rect->y < bounds.y) rect->y = bounds.y;
        if ((int64_t)rect->y > maximum) {
            rect->y = clamp_i64_to_i32(maximum);
        }
    }
}

/* Enforce minimum dimensions while keeping the opposite resize edge fixed. */
static void constrain_minimum_size(UmiDesignerSurfaceInteraction *interaction)
{
    UmiDesignerRect *rect = &interaction->preview;
    int64_t right = (int64_t)rect->x + rect->width;
    int64_t bottom = (int64_t)rect->y + rect->height;

    if (rect->width < interaction->minimum_width) {
        rect->width = interaction->minimum_width;
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_LEFT) != 0U) {
            rect->x = clamp_i64_to_i32(right - rect->width);
        }
    }
    if (rect->height < interaction->minimum_height) {
        rect->height = interaction->minimum_height;
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_TOP) != 0U) {
            rect->y = clamp_i64_to_i32(bottom - rect->height);
        }
    }
}

/* Clip only the edges being resized, leaving the opposite edge stable. */
static void constrain_resize_to_canvas(
    UmiDesignerSurfaceInteraction *interaction)
{
    UmiDesignerRect *rect = &interaction->preview;
    UmiDesignerRect bounds = interaction->canvas_bounds;
    int64_t right = (int64_t)rect->x + rect->width;
    int64_t bottom = (int64_t)rect->y + rect->height;
    int64_t canvas_right = (int64_t)bounds.x + bounds.width;
    int64_t canvas_bottom = (int64_t)bounds.y + bounds.height;

    if (bounds.width > 0 && rect->x < bounds.x) {
        rect->width = clamp_i64_to_i32(right - bounds.x);
        rect->x = bounds.x;
    }
    right = (int64_t)rect->x + rect->width;
    if (bounds.width > 0 && right > canvas_right) {
        rect->width = clamp_i64_to_i32(canvas_right - rect->x);
    }
    if (bounds.height > 0 && rect->y < bounds.y) {
        rect->height = clamp_i64_to_i32(bottom - bounds.y);
        rect->y = bounds.y;
    }
    bottom = (int64_t)rect->y + rect->height;
    if (bounds.height > 0 && bottom > canvas_bottom) {
        rect->height = clamp_i64_to_i32(canvas_bottom - rect->y);
    }
    constrain_minimum_size(interaction);
}

/* Snap one signed coordinate with wide arithmetic so a pointer near an integer
 * limit cannot overflow while rounding to the nearest grid line. */
static int32_t snap_coordinate(int32_t value, unsigned grid_size)
{
    int64_t grid = (int64_t)grid_size;
    int64_t remainder;
    int64_t base;

    if (grid_size == 0U) return value;
    remainder = (int64_t)value % grid;
    if (remainder < 0) remainder += grid;
    base = (int64_t)value - remainder;
    if (remainder * 2 >= grid) base += grid;
    return clamp_i64_to_i32(base);
}

/* Snap only the edges controlled by the pointer. This preserves the opposite
 * edge instead of rounding position and size as unrelated values. */
static void snap_resize_edges(UmiDesignerSurfaceInteraction *interaction)
{
    UmiDesignerRect before_snap = interaction->preview;
    UmiDesignerRect *rect = &interaction->preview;
    int64_t fixed_right = (int64_t)rect->x + rect->width;
    int64_t fixed_bottom = (int64_t)rect->y + rect->height;

    if (!interaction->options.snap_to_grid) return;
    if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_LEFT) != 0U) {
        rect->x = snap_coordinate(rect->x, interaction->options.grid_size);
        rect->width = clamp_i64_to_i32(fixed_right - rect->x);
    } else if ((interaction->edges &
                UMI_DESIGNER_SURFACE_EDGE_RIGHT) != 0U) {
        int32_t right = snap_coordinate(
            clamp_i64_to_i32(fixed_right),
            interaction->options.grid_size);

        rect->width = clamp_i64_to_i32((int64_t)right - rect->x);
    }
    if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_TOP) != 0U) {
        rect->y = snap_coordinate(rect->y, interaction->options.grid_size);
        rect->height = clamp_i64_to_i32(fixed_bottom - rect->y);
    } else if ((interaction->edges &
                UMI_DESIGNER_SURFACE_EDGE_BOTTOM) != 0U) {
        int32_t bottom = snap_coordinate(
            clamp_i64_to_i32(fixed_bottom),
            interaction->options.grid_size);

        rect->height = clamp_i64_to_i32((int64_t)bottom - rect->y);
    }
    interaction->snapped_x =
        rect->x != before_snap.x || rect->width != before_snap.width;
    interaction->snapped_y =
        rect->y != before_snap.y || rect->height != before_snap.height;
}

/* Recalculate preview geometry from the original rectangle on every update.
 * This avoids accumulating rounding error across many pointer events. */
UmiStatus umi_designer_surface_interaction_update(
    UmiDesignerSurfaceInteraction *interaction,
    int32_t pointer_x,
    int32_t pointer_y)
{
    int64_t delta_x;
    int64_t delta_y;
    UmiDesignerRect requested;

    if (interaction == NULL || !interaction->active ||
        interaction->structure_size != (uint32_t)sizeof(*interaction) ||
        interaction->api_version !=
            UMI_DESIGNER_SURFACE_INTERACTION_API_VERSION) {
        return UMI_STATUS_INVALID_STATE;
    }
    delta_x = (int64_t)pointer_x - interaction->pointer_start_x;
    delta_y = (int64_t)pointer_y - interaction->pointer_start_y;
    requested = interaction->before;

    if (interaction->kind == UMI_DESIGNER_SURFACE_INTERACTION_MOVE) {
        requested.x = clamp_i64_to_i32((int64_t)requested.x + delta_x);
        requested.y = clamp_i64_to_i32((int64_t)requested.y + delta_y);
        interaction->preview = requested;
        if (interaction->options.snap_to_grid) {
            interaction->preview.x = snap_coordinate(
                requested.x,
                interaction->options.grid_size);
            interaction->preview.y = snap_coordinate(
                requested.y,
                interaction->options.grid_size);
        }
        interaction->snapped_x = interaction->preview.x != requested.x;
        interaction->snapped_y = interaction->preview.y != requested.y;
        constrain_move(interaction->canvas_bounds, &interaction->preview);
    } else {
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_LEFT) != 0U) {
            requested.x = clamp_i64_to_i32((int64_t)requested.x + delta_x);
            requested.width = clamp_i64_to_i32(
                (int64_t)requested.width - delta_x);
        }
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_RIGHT) != 0U) {
            requested.width = clamp_i64_to_i32(
                (int64_t)requested.width + delta_x);
        }
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_TOP) != 0U) {
            requested.y = clamp_i64_to_i32((int64_t)requested.y + delta_y);
            requested.height = clamp_i64_to_i32(
                (int64_t)requested.height - delta_y);
        }
        if ((interaction->edges & UMI_DESIGNER_SURFACE_EDGE_BOTTOM) != 0U) {
            requested.height = clamp_i64_to_i32(
                (int64_t)requested.height + delta_y);
        }
        interaction->preview = requested;
        constrain_minimum_size(interaction);
        snap_resize_edges(interaction);
        constrain_minimum_size(interaction);
        constrain_resize_to_canvas(interaction);
    }
    interaction->changed = !rect_equal(
        interaction->before,
        interaction->preview);
    return UMI_STATUS_OK;
}

/* Add one integer geometry value to a copied semantic node. */
static UmiStatus set_integer_attribute(
    UmiDeclNode *node,
    const char *name,
    int32_t value)
{
    char text[32];
    int written;

    written = snprintf(text, sizeof(text), "%" PRId32, value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_decl_node_set_attribute(
        node,
        name,
        UMI_DECL_VALUE_INTEGER,
        text);
}

/* Copy a rectangle into a node before the node is submitted to history. */
static UmiStatus set_node_rect(UmiDeclNode *node, UmiDesignerRect rect)
{
    UmiStatus status;

    if (node == NULL || rect.width < 0 || rect.height < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = set_integer_attribute(node, "x", rect.x);
    if (status == UMI_STATUS_OK) {
        status = set_integer_attribute(node, "y", rect.y);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer_attribute(node, "width", rect.width);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer_attribute(node, "height", rect.height);
    }
    return status;
}

/* Apply the completed preview once, after checking that its source is current. */
UmiStatus umi_designer_surface_interaction_commit(
    UmiDesignerSurfaceInteraction *interaction,
    UmiDesignerDocument *document,
    UmiDesignerHistory *history)
{
    UmiDesignerRect current_rect;
    UmiDeclNode before;
    UmiDeclNode after;
    UmiDesignerOperation operation;
    UmiStatus status;

    if (interaction == NULL || document == NULL || history == NULL ||
        !interaction->active ||
        interaction->structure_size != (uint32_t)sizeof(*interaction) ||
        interaction->api_version !=
            UMI_DESIGNER_SURFACE_INTERACTION_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!interaction->changed) {
        interaction->active = 0;
        return UMI_STATUS_OK;
    }
    status = umi_designer_surface_get_rect(
        document,
        interaction->node_id,
        &current_rect);
    /* A stale gesture must not overwrite a geometry change made elsewhere. */
    if (status == UMI_STATUS_OK &&
        !rect_equal(current_rect, interaction->before)) {
        return UMI_STATUS_BUSY;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_decl_document_find_node(
            umi_designer_document_declarative(document),
            interaction->node_id,
            &before);
    }
    if (status == UMI_STATUS_OK) {
        after = before;
        status = set_node_rect(&after, interaction->preview);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_designer_operation_move(&before, &after, &operation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_designer_history_execute(history, &operation);
    }
    if (status == UMI_STATUS_OK) interaction->active = 0;
    return status;
}

/* End a gesture locally; no document operation is needed because previews are
 * intentionally stored only in the interaction record. */
void umi_designer_surface_interaction_cancel(
    UmiDesignerSurfaceInteraction *interaction)
{
    if (interaction == NULL) return;
    interaction->preview = interaction->before;
    interaction->changed = 0;
    interaction->active = 0;
}
