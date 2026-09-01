/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/surface.c
 *
 * PURPOSE:
 *   Map semantic geometry to document properties and implement reusable grid,
 *   zoom, alignment and distribution mathematics for designer hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These functions never manipulate GTK widgets. They operate on semantic
 * rectangles so every future frontend receives exactly the same layout rules.
 */

#include "umicom/designer/surface.h"

#include <stdio.h>
#include <stdlib.h>

static int attr_int(const UmiDeclNode *node,
                    const char *name,
                    int fallback)
{
    UmiDeclAttribute attribute;

    if (umi_decl_node_get_attribute(node, name, &attribute) != UMI_STATUS_OK) {
        return fallback;
    }

    return (int)strtol(attribute.value.text, NULL, 10);
}

UmiStatus umi_designer_surface_get_rect(
    const UmiDesignerDocument *document,
    const char *id,
    UmiDesignerRect *out_rect)
{
    UmiDeclNode node;
    UmiStatus status;

    if (document == NULL || id == NULL || out_rect == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        id,
        &node
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_rect->x = attr_int(&node, "x", 0);
    out_rect->y = attr_int(&node, "y", 0);
    out_rect->width = attr_int(&node, "width", 120);
    out_rect->height = attr_int(&node, "height", 40);
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_surface_set_rect(
    UmiDesignerDocument *document,
    const char *id,
    UmiDesignerRect rect)
{
    UmiDeclNode node;
    char value[32];
    UmiStatus status;

    if (document == NULL || id == NULL ||
        rect.width < 0 || rect.height < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_decl_document_find_node(
        umi_designer_document_declarative(document),
        id,
        &node
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)snprintf(value, sizeof(value), "%d", rect.x);
    status = umi_decl_node_set_attribute(&node,
                                         "x",
                                         UMI_DECL_VALUE_INTEGER,
                                         value);
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.y);
        status = umi_decl_node_set_attribute(&node,
                                             "y",
                                             UMI_DECL_VALUE_INTEGER,
                                             value);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.width);
        status = umi_decl_node_set_attribute(&node,
                                             "width",
                                             UMI_DECL_VALUE_INTEGER,
                                             value);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(value, sizeof(value), "%d", rect.height);
        status = umi_decl_node_set_attribute(&node,
                                             "height",
                                             UMI_DECL_VALUE_INTEGER,
                                             value);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_decl_document_update_node(
            umi_designer_document_declarative(document),
            &node
        );
    }
    if (status == UMI_STATUS_OK) {
        umi_designer_document_mark_changed(document);
    }

    return status;
}

void umi_designer_surface_options_init(UmiDesignerSurfaceOptions *options)
{
    if (options == NULL) {
        return;
    }

    options->grid_size = 8U;
    options->zoom = 1.0F;
    options->show_grid = 1;
    options->snap_to_grid = 1;
    options->show_guides = 1;
}

UmiStatus umi_designer_surface_set_zoom(
    UmiDesignerSurfaceOptions *options,
    float zoom)
{
    if (options == NULL || zoom < 0.25F || zoom > 4.0F) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    options->zoom = zoom;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_surface_set_grid(
    UmiDesignerSurfaceOptions *options,
    unsigned grid_size,
    int snap_to_grid)
{
    if (options == NULL || grid_size == 0U || grid_size > 256U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    options->grid_size = grid_size;
    options->snap_to_grid = snap_to_grid != 0;
    return UMI_STATUS_OK;
}

static int snap_value(int value, unsigned grid)
{
    int grid_value = (int)grid;
    int remainder;

    if (grid == 0U) {
        return value;
    }

    remainder = value % grid_value;
    if (remainder < 0) {
        remainder += grid_value;
    }

    return remainder * 2 >= grid_value
        ? value + (grid_value - remainder)
        : value - remainder;
}

UmiDesignerRect umi_designer_surface_snap_rect(
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerRect rect)
{
    if (options == NULL || !options->snap_to_grid ||
        options->grid_size == 0U) {
        return rect;
    }

    rect.x = snap_value(rect.x, options->grid_size);
    rect.y = snap_value(rect.y, options->grid_size);
    rect.width = snap_value(rect.width, options->grid_size);
    rect.height = snap_value(rect.height, options->grid_size);
    return rect;
}

UmiStatus umi_designer_surface_align(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerAlignment alignment)
{
    size_t index;
    int target;

    if (rects == NULL || count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (alignment) {
        case UMI_DESIGNER_ALIGN_LEFT:
            target = rects[0].x;
            for (index = 1U; index < count; ++index) {
                if (rects[index].x < target) {
                    target = rects[index].x;
                }
            }
            for (index = 0U; index < count; ++index) {
                rects[index].x = target;
            }
            break;

        case UMI_DESIGNER_ALIGN_RIGHT:
            target = rects[0].x + rects[0].width;
            for (index = 1U; index < count; ++index) {
                int edge = rects[index].x + rects[index].width;
                if (edge > target) {
                    target = edge;
                }
            }
            for (index = 0U; index < count; ++index) {
                rects[index].x = target - rects[index].width;
            }
            break;

        case UMI_DESIGNER_ALIGN_TOP:
            target = rects[0].y;
            for (index = 1U; index < count; ++index) {
                if (rects[index].y < target) {
                    target = rects[index].y;
                }
            }
            for (index = 0U; index < count; ++index) {
                rects[index].y = target;
            }
            break;

        case UMI_DESIGNER_ALIGN_BOTTOM:
            target = rects[0].y + rects[0].height;
            for (index = 1U; index < count; ++index) {
                int edge = rects[index].y + rects[index].height;
                if (edge > target) {
                    target = edge;
                }
            }
            for (index = 0U; index < count; ++index) {
                rects[index].y = target - rects[index].height;
            }
            break;

        case UMI_DESIGNER_ALIGN_HORIZONTAL_CENTRE:
            target = rects[0].x + rects[0].width / 2;
            for (index = 0U; index < count; ++index) {
                rects[index].x = target - rects[index].width / 2;
            }
            break;

        case UMI_DESIGNER_ALIGN_VERTICAL_CENTRE:
            target = rects[0].y + rects[0].height / 2;
            for (index = 0U; index < count; ++index) {
                rects[index].y = target - rects[index].height / 2;
            }
            break;

        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

static void sort_horizontal(UmiDesignerRect *rects, size_t count)
{
    size_t outer;
    size_t inner;

    for (outer = 0U; outer + 1U < count; ++outer) {
        for (inner = outer + 1U; inner < count; ++inner) {
            if (rects[inner].x < rects[outer].x) {
                UmiDesignerRect temporary = rects[outer];
                rects[outer] = rects[inner];
                rects[inner] = temporary;
            }
        }
    }
}

static void sort_vertical(UmiDesignerRect *rects, size_t count)
{
    size_t outer;
    size_t inner;

    for (outer = 0U; outer + 1U < count; ++outer) {
        for (inner = outer + 1U; inner < count; ++inner) {
            if (rects[inner].y < rects[outer].y) {
                UmiDesignerRect temporary = rects[outer];
                rects[outer] = rects[inner];
                rects[inner] = temporary;
            }
        }
    }
}

UmiStatus umi_designer_surface_distribute(
    UmiDesignerRect *rects,
    size_t count,
    UmiDesignerDistribution distribution)
{
    size_t index;
    int available;
    int occupied = 0;
    int gap;

    if (rects == NULL || count < 3U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (distribution == UMI_DESIGNER_DISTRIBUTE_HORIZONTAL) {
        sort_horizontal(rects, count);
        available = rects[count - 1U].x + rects[count - 1U].width -
                    rects[0].x;
        for (index = 0U; index < count; ++index) {
            occupied += rects[index].width;
        }
        gap = (available - occupied) / (int)(count - 1U);
        for (index = 1U; index + 1U < count; ++index) {
            rects[index].x =
                rects[index - 1U].x + rects[index - 1U].width + gap;
        }
        return UMI_STATUS_OK;
    }

    if (distribution == UMI_DESIGNER_DISTRIBUTE_VERTICAL) {
        sort_vertical(rects, count);
        available = rects[count - 1U].y + rects[count - 1U].height -
                    rects[0].y;
        for (index = 0U; index < count; ++index) {
            occupied += rects[index].height;
        }
        gap = (available - occupied) / (int)(count - 1U);
        for (index = 1U; index + 1U < count; ++index) {
            rects[index].y =
                rects[index - 1U].y + rects[index - 1U].height + gap;
        }
        return UMI_STATUS_OK;
    }

    return UMI_STATUS_INVALID_ARGUMENT;
}
