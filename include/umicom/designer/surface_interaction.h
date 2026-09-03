/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/surface_interaction.h
 *
 * PURPOSE:
 *   Turn pointer gestures into bounded, snapped and undoable visual-designer
 *   geometry changes without depending on a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A frontend supplies pointer coordinates and renders the preview rectangle.
 * Framework owns hit-testing, size limits, canvas limits, snapping and the
 * final history operation, so every desktop or web adapter behaves alike.
 */

#ifndef UMICOM_DESIGNER_SURFACE_INTERACTION_H
#define UMICOM_DESIGNER_SURFACE_INTERACTION_H

#include "umicom/designer/history.h"
#include "umicom/designer/selection.h"
#include "umicom/designer/surface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_SURFACE_INTERACTION_API_VERSION 2U
#define UMI_DESIGNER_SURFACE_DEFAULT_HANDLE_SIZE 7
#define UMI_DESIGNER_SURFACE_DEFAULT_MINIMUM_SIZE 16

/** Describe whether a pointer gesture moves a component or resizes its edges. */
typedef enum UmiDesignerSurfaceInteractionKind {
    UMI_DESIGNER_SURFACE_INTERACTION_NONE = 0,
    UMI_DESIGNER_SURFACE_INTERACTION_MOVE = 1,
    UMI_DESIGNER_SURFACE_INTERACTION_RESIZE = 2
} UmiDesignerSurfaceInteractionKind;

/**
 * Identify the component edges controlled by a resize gesture. Corner handles
 * combine one horizontal flag with one vertical flag.
 */
typedef enum UmiDesignerSurfaceEdge {
    UMI_DESIGNER_SURFACE_EDGE_NONE = 0,
    UMI_DESIGNER_SURFACE_EDGE_LEFT = 1U << 0,
    UMI_DESIGNER_SURFACE_EDGE_TOP = 1U << 1,
    UMI_DESIGNER_SURFACE_EDGE_RIGHT = 1U << 2,
    UMI_DESIGNER_SURFACE_EDGE_BOTTOM = 1U << 3
} UmiDesignerSurfaceEdge;

/** Return stable hit-test evidence that a frontend can use to choose a cursor. */
typedef struct UmiDesignerSurfaceHit {
    uint32_t structure_size;
    uint32_t api_version;
    char node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerRect rect;
    unsigned edges;
    int found;
} UmiDesignerSurfaceHit;

/**
 * Hold one gesture from pointer press until commit or cancellation. The
 * semantic document is not changed while the pointer is moving.
 */
typedef struct UmiDesignerSurfaceInteraction {
    uint32_t structure_size;
    uint32_t api_version;
    char node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerSurfaceInteractionKind kind;
    unsigned edges;
    UmiDesignerSurfaceOptions options;
    UmiDesignerRect canvas_bounds;
    UmiDesignerRect before;
    UmiDesignerRect preview;
    int32_t pointer_start_x;
    int32_t pointer_start_y;
    int32_t minimum_width;
    int32_t minimum_height;
    int active;
    int changed;
    int snapped_x;
    int snapped_y;
} UmiDesignerSurfaceInteraction;

/**
 * Insert a new semantic component at a requested canvas rectangle. Framework
 * snaps and bounds the rectangle before recording the complete node as one
 * undoable history operation, so palette drops behave consistently in every
 * frontend.
 */
UmiStatus umi_designer_surface_insert_component(
    UmiDesignerDocument *document,
    UmiDesignerHistory *history,
    const char *node_id,
    const char *component_type,
    const char *parent_id,
    UmiDesignerRect requested_rect,
    UmiDesignerRect canvas_bounds,
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerRect *out_rect);

/**
 * Find the topmost component below a canvas point. A non-zero handle size also
 * reports nearby resize edges for the component that was found.
 */
UmiStatus umi_designer_surface_hit_test(
    const UmiDesignerDocument *document,
    int32_t x,
    int32_t y,
    int32_t handle_size,
    UmiDesignerSurfaceHit *out_hit);

/**
 * Begin a move or resize without mutating the document. Canvas bounds with a
 * non-positive width or height intentionally leave that axis unbounded.
 */
UmiStatus umi_designer_surface_interaction_begin(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerSurfaceInteractionKind kind,
    unsigned edges,
    int32_t pointer_x,
    int32_t pointer_y,
    UmiDesignerRect canvas_bounds,
    const UmiDesignerSurfaceOptions *options,
    UmiDesignerSurfaceInteraction *out_interaction);

/** Update the preview rectangle for the current pointer location. */
UmiStatus umi_designer_surface_interaction_update(
    UmiDesignerSurfaceInteraction *interaction,
    int32_t pointer_x,
    int32_t pointer_y);

/**
 * Commit the preview as one undoable semantic operation. A stale interaction
 * is rejected when another operation changed the same rectangle first.
 */
UmiStatus umi_designer_surface_interaction_commit(
    UmiDesignerSurfaceInteraction *interaction,
    UmiDesignerDocument *document,
    UmiDesignerHistory *history);

/** Cancel a gesture and restore its preview to the original rectangle. */
void umi_designer_surface_interaction_cancel(
    UmiDesignerSurfaceInteraction *interaction);

#ifdef __cplusplus
}
#endif

#endif
